#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

	class GDExample : public Node {
		GDCLASS(GDExample, Node)

	private:
		double time_passed;

	protected:
		static void _bind_methods();

	public:
		GDExample();
		~GDExample();
		void _ready() override;
		void _process(double delta) override;
	};

}

#endif