#include "upgrade.h"

#include "varint_decode.h"
#include "esp_log.h"
#include "can_driver.h"
#include "device.h"

using namespace upgrade;

const char * TAG = "Upgrade";
static Handler handler;

class OtaReceiver : public Receiver {
private:
	const esp_partition_t* otaPartition;
	esp_ota_handle_t update_handle;

	bool partitionValid(void);
public:
	bool init(Ginco__Command* command);
	bool receive(const uint8_t * data, uint32_t len);
	void complete();
	void fail();
};

class CanReceiver : public Receiver {
private:
	driver::can::message_t can_message;
public:
	bool init(Ginco__Command* command);
	bool receive(const uint8_t * data, uint32_t len);
	void complete();
	void fail();
};

static CanReceiver can_receiver;
static OtaReceiver ota_receiver;
static Receiver* active_receiver;


Handler::Handler(void){};

void Handler::init(const esp_mqtt_event_t& event){
	uint32_t protoBufLength;
	uint8_t * ptr = (uint8_t *) event.data;
	uint32_t processed;

	ptr = smp_varint_decode(ptr, &protoBufLength);
	processed = smp_varint_encoding_length(protoBufLength) + protoBufLength;
	Ginco__Command* command = ginco__command__unpack(NULL, protoBufLength, ptr);
	ptr += protoBufLength;
	if (!command)
		return;

	if (command->upgrade->device_id == 0){
		active_receiver = &ota_receiver;
	} else {
		active_receiver = &can_receiver;
	}

	if(!active_receiver->init(command)){
		active_receiver->fail();
	} else {
		active_receiver->receive(ptr, event.data_len - processed);
	}
    ginco__command__free_unpacked(command, NULL);
};

void Handler::handle(const esp_mqtt_event_t & event) {
	active_receiver->receive(reinterpret_cast<uint8_t *> (event.data), event.data_len);
};

void Handler::end(){
	active_receiver->complete();
};


bool OtaReceiver::init(Ginco__Command * command) {
	otaPartition = esp_ota_get_next_update_partition(NULL);
	if (otaPartition == NULL){
		ESP_LOGE(TAG, "Passive OTA partition not found");
		return false;
	}
	if (command->upgrade->image_size > otaPartition->size){
		ESP_LOGE(TAG, "Image size too large");
		return false;
	}

	ESP_ERROR_CHECK(esp_ota_begin(otaPartition, command->upgrade->image_size, &update_handle));
	ESP_LOGI(TAG, "esp_ota_begin succeeded");
	return true;
};

bool OtaReceiver::partitionValid(void){
	esp_app_desc_t newDescription;
	const esp_app_desc_t * currentDescription = esp_app_get_description();
	esp_err_t err = esp_ota_get_partition_description(otaPartition, &newDescription);
	if (err != ESP_OK) {
		ESP_LOGW(TAG, "ota get description returned %d ", err);
		return false;
	}
	if (strcmp(currentDescription->project_name, newDescription.project_name)) {
		ESP_LOGW(TAG, "Invalid image name %s, should be %s", newDescription.project_name, currentDescription->project_name);
		return false;
	}
	return true;
};

bool OtaReceiver::receive(const uint8_t * data, uint32_t len) {
	ESP_ERROR_CHECK(esp_ota_write(update_handle, data, len));
	return true;
};

void OtaReceiver::complete(){
	ESP_ERROR_CHECK(esp_ota_end(update_handle));
	if (!partitionValid())
		return;

	ESP_ERROR_CHECK(esp_ota_set_boot_partition(otaPartition));
	ESP_LOGI(TAG, "esp_ota_set_boot_partition succeeded");
	esp_restart();
};

void OtaReceiver::fail(){
	if(update_handle)
		esp_ota_end(update_handle);
};


bool CanReceiver::init(Ginco__Command * command) {
	const esp_partition_t* otaPartition = esp_ota_get_next_update_partition(NULL);
	if (otaPartition == NULL){
		ESP_LOGE(TAG, "Passive OTA partition not found");
		return false;
	}
	if (command->upgrade->image_size > otaPartition->size){
		ESP_LOGE(TAG, "Image size too large");
		return false;
	}
	device::init_can_message(&can_message);
	return true;
};

bool CanReceiver::receive(const uint8_t* data, uint32_t len) {
	uint64_t buffer;
	uint8_t retries = {0};
	while (len > 8){
		len -= 8;
		buffer = 0;
		for (int i=0; i < 8; i++){
			buffer += (*data << 8*i);
			++data;
		}
		can_message.data = buffer;
		can_message.buffer_size = 8;
		while (!driver::can::transmit(can_message, true)){
			retries += 1;
			ESP_LOGI(TAG, "Retry for upgrade %u", retries);
		}
	}
	buffer = 0;
	for (int i=0; i < 8; i++){
		buffer += (*data << 8*i);
		++data;
	}
	can_message.data = buffer;
	can_message.buffer_size = 8;
	while (!driver::can::transmit(can_message, true)){
		retries += 1;
		ESP_LOGI(TAG, "Retry for upgrade %u", retries);
	}

	return true;
};

void CanReceiver::complete(){
	fail();
};

void CanReceiver::fail(){
	// Transmit ending message
	driver::can::transmit(can_message, true);
};