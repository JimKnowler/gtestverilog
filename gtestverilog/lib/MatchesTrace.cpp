#include "MatchesTrace.h"

#include <algorithm>

namespace gtestverilog {
namespace matches_trace {
    const size_t kMaxTraceRenderSize = 80;

    bool compare(const Trace& actual, const Trace& expected, ::testing::MatchResultListener& listener) {
        bool hasMatched = true;

        auto& stepsActual = actual.getSteps();
        auto& stepsExpected = expected.getSteps();
        
        const uint32_t sizeActual = stepsActual.size();
        const uint32_t sizeExpected = stepsExpected.size();

        if (sizeActual != sizeExpected) {
            if (listener.IsInterested()) {
                listener << "different number of steps: expected [" << sizeExpected << "] != actual [" << sizeActual << "]\n";
            }
            return false;
        }

        const uint64_t portMaskActual = actual.getPortMask();
        const uint64_t portMaskExpected = expected.getPortMask();

        const uint64_t portMaskOverlap = portMaskActual & portMaskExpected;

        for (uint32_t portId=0; portId<64; portId++) {
            if (0 == (portMaskOverlap & (uint64_t(1) << portId))) {
                continue;
            }

            const PortDescription& portDesc = actual.getPortDescription(portId);
            
            for (size_t step=0; step < sizeActual; step++) {
                const PortValue& valueActual = stepsActual[step].port(portDesc);
                const PortValue& valueExpected = stepsExpected[step].port(portDesc);

                if (valueActual != valueExpected) {
                    if (listener.IsInterested()) {
                        listener << "\nunexpected signal on " 
                                << ConsoleColour().fg(Trace::getColourForPortId(portId))
                                << portDesc.label()
                                << ConsoleColour().reset()
                                << "\n";

                        std::vector<Step> stepsExpectedRender;
                        std::vector<Step> stepsActualRender;

                        size_t stepStart = 0;
                        size_t stepEnd = sizeActual;

                        if (sizeActual > kMaxTraceRenderSize) {
                            stepStart = (step > (kMaxTraceRenderSize/2)) ? step - (kMaxTraceRenderSize/2) : 0;
                            stepEnd = std::min(stepEnd, stepStart + kMaxTraceRenderSize);

                            listener << "Note: Diff truncated to steps " << stepStart << " to " << stepEnd << "\n";

                            stepsExpectedRender = std::vector<Step>(stepsExpected.begin() + stepStart, stepsExpected.begin() + stepEnd);
                            stepsActualRender = std::vector<Step>(stepsActual.begin() + stepStart, stepsActual.begin() + stepEnd);
                        } else {
                            stepsExpectedRender = stepsExpected;
                            stepsActualRender = stepsActual;
                        }

                        size_t maxPortLabelSize = expected.getMaxPortLabelSize();

                        listener << "\n";
                        listener << "Expected:";
                        Trace::renderPortDiff(*(listener.stream()), 'v', ConsoleColour::kGreen, maxPortLabelSize-9, portDesc, stepsExpectedRender, stepsActualRender);

                        Trace::renderPort(*(listener.stream()), maxPortLabelSize, portDesc, stepsExpectedRender);

                        listener << "  Actual:";
                        Trace::renderPortDiff(*(listener.stream()), 'v', ConsoleColour::kRed, maxPortLabelSize-9, portDesc, stepsExpectedRender, stepsActualRender);

                        Trace::renderPort(*(listener.stream()), maxPortLabelSize, portDesc, stepsActualRender);
                    }

                    hasMatched = false;

                    break;
                }
                
            }
        }

        return hasMatched;
    }
    
} // matches_trace
} // gtestverilog