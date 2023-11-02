#pragma once

namespace utils {

    /*
     *  Message is the queue item type used in StandardTask
     *
     *  Using a FreeRtos queue in C++ raises some issues.
     *  We are severely limited to the type of obhects we can pass.
     *  (In cpp terminology they must be TriviallyCopyable, that is we can copy them using memcpy)
     *
     *  Messages consists of an integer event and a variant value that details the event.
     *
     *  The value is either
     *
     *    a uint32_t
     *    a normal pointer
     *    a pointer to a "new object" obtained from a std::unique_ptr
     *
     *  The current implementation is type safe for the 3 variants,
     *  but not for casting the void pointer to a type pointer,
     *  which may change in the future
     *
     */
    class Message {

    private:
        // wrapper structs to differentiate the two kinds of void *
        struct Pointer {
            void * p_;
        };
        struct UniquePointer {
            void * p_;
        };

        // used after conversion of UniquePointer to unique_ptr
        struct InvalidPointer {
            void * p_;
        };

        uint32_t event_;
        std::variant<uint32_t, Pointer , UniquePointer, InvalidPointer> value_;

        /**
         * we make the constructor private and only accessible by friend class StandardTask
         * to make sure the message API in StandardTask is used
         */
        template <typename T>
        Message(uint32_t event, T t) : event_{event} , value_{t} {}


        /**
         * if type match, obtain the value of the current type
         *
         * First check the correct type, otherwise std::variant throws which would end the program
         *  because we compile with noexceptions.
         *
         */
        template <typename T>
        T value() const {
            bool variant_ok = std::holds_alternative<T>(value_);
            assert(variant_ok);
            return variant_ok ? std::get<T>(value_) : T{0};
        }

    public:
        // pubic zero argument constructor is required for Queue (the FreeRtoss Queue wrapper class)
        Message() : event_{0} , value_{0UL} {}

        // API for message consumers

        uint32_t event() const { return event_; }

        uint32_t uint32Value() const { return value<uint32_t>(); }

        /*
         * can only be used to cast a "raw pointer" that was not wrapped in a unique_ptr
         */
        template <typename T>
        T * pointerValue() const { return static_cast<T *> (value<Pointer>().p_); }

        /*
        * take ownership of the unique pointer if any
        */
        template <typename T>
        std::unique_ptr<T> takeValue()
        {
            auto p = std::unique_ptr<T>(static_cast<T *>(value<UniquePointer>().p_));
            // can take only once, so set pointer to null,
            // making sure all further attempts assert by setting an unused type
            value_ = InvalidPointer {nullptr};
            return p;
        }

        /*
         * used to check if the message consumer took ownership of the unique pointer
         */
        bool hasUniquePointer() { return std::holds_alternative<UniquePointer>(value_); }

        friend class StandardTask;
    };

    /**
     *  just a reminder for the maintainer that the message size should be relatively small
     *  No code really depends on this
     *
     */
    static_assert(sizeof(Message) <= 12, "Message size is rather big");

}
