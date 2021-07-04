- bazel build //gtestverilog:test  && ./bazel-bin/gtestverilog/test --gtest_filter="*"

- bazel build //gtestverilog:example --incompatible_require_linker_input_cc_api=false  && ./bazel-bin/gtestverilog/example --gtest_filter="*"

