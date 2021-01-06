//
// based on https://zipcpu.com/blog/2017/06/21/looking-at-verilator.html
//

#include <cstdint>
#include <memory>

namespace gtestverilog {

	/// @class TestBench
	/// @brief Templated base class for simulating a verilated verilog module
	template<class MODULE>
	class TestBench {
	public:
		TestBench(void) {
			m_core = std::make_unique<MODULE>();
			
			m_stepCount = 0;	
			
			m_core->i_reset_n = 1;
			
			setClockPolarity(0);
		}

		~TestBench(void) {
			m_core.release();
		}

		/// @brief set the 'i_reset_n' port low and simulate a clock tick
		void reset(void) {
			m_core->i_reset_n = 0;
			tick();
			m_core->i_reset_n = 1;
		}

		/// @brief simulate a clock cycle
		/// @note a clock cycle has two steps - first step has port 'i_clk'=1, the second step has port 'i_clk'=0
		void tick(size_t numTicks = 1) {
			for (size_t i=0; i<numTicks; i++) {
				// rising edge
				assert(m_core->i_clk == m_clockPolarity);
				step();

				// falling edge
				assert(m_core->i_clk != m_clockPolarity);
				step();
			}
		}

		/// @brief simulate a single half clock step
		/// @note This will invert the current value on port 'i_clk'
		void step() {
			// notify virtual function - to allow any simulatino code to run with current clock value
			onStepSimulateCombinatorial();

			// evaluate core to allow combinatorial values to settle first
			m_core->eval();

			// invert clock
			m_core->i_clk = (m_core->i_clk) ? 0 : 1;

			// notify virtual function - to allow any simulation code to run with the new clock value
			onStepSimulateSequential();

			// evaluate the core
			m_core->eval();

			// step is complete
			m_stepCount += 1;

			// notify virtual function - that step has completed
			onStep();
		}

		MODULE& core() {
			return *m_core;
		}

		uint64_t stepCount() const {
			return m_stepCount;
		}

		/// @brief set the polarity of i_clk
		/// @note clock polarity is defined here as the value of i_clk in the 
		///       second half-cycle (step) of each tick
		/// @value polarity the polarity value to apply
		void setClockPolarity(int polarity) {
			assert((polarity==1) || (polarity==0));
			m_clockPolarity = polarity;

			m_core->i_clk = m_clockPolarity;
			m_core->eval();
		}

	protected:
		virtual void onStepSimulateCombinatorial() {}
		virtual void onStepSimulateSequential() {}
		virtual void onStep() {}

	private:
		std::unique_ptr<MODULE>		m_core;

		uint64_t    m_stepCount;
		int 		m_clockPolarity;
	};

}