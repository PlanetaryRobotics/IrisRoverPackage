#include <GTestBase.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public ExampleComponents::TestComponentGTestBase {
public:
#if FW_OBJECT_NAMES == 1
    ATester(void) : ExampleComponents::TestComponentGTestBase("comp",10) {
#else
        ATester(void) : ExampleComponents::TestComponentGTestBase(10)  {
#endif
        }

        void from_testOut_handler(
                const NATIVE_INT_TYPE portNum, //!< The port number
                I32 arg1, //!< A built-in type argument
                AnotherExample::arg2String &arg2, //!< A string argument
                AnotherExample::arg3Buffer arg3 //!< A buffer argument
        );
    };


    void ATester::from_testOut_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            I32 arg1, //!< A built-in type argument
            AnotherExample::arg2String &arg2, //!< A string argument
            AnotherExample::arg3Buffer arg3 //!< A buffer argument
    ) {

    }


    int main(int argc, char* argv[]) {

        ATester testBase;

    }
