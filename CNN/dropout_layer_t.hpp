#pragma once
#include "layer_t.h"

class dropout_layer_t : public layer_t
{
public:
	tensor_t<bool> hitmap;
	const float p_activation;

	dropout_layer_t( tdsize in_size, float p_activation )
		:
		layer_t(in_size, in_size),
		hitmap( in_size.x, in_size.y, in_size.z ),
		p_activation( p_activation )
		{
			throw_assert(p_activation >= 0 && p_activation <= 1.0, "activation level should be betwene 0.0 and 1.0");
		}

	size_t get_total_memory_size() const {
		return hitmap.get_total_memory_size() + layer_t::get_total_memory_size();
	}
	
	std::string kind_str() const {
		return "dropout";
	}
	std::string param_str() const {
		std::stringstream ss;
		ss << "activation=" << p_activation;
		return ss.str();
	}
	
	bool operator==(const dropout_layer_t & o) const {
		if (o.p_activation != p_activation) return false;
		if (o.hitmap != hitmap) return false;
		if (o.in != in) return false;
		if (o.grads_in != grads_in) return false;
		if (o.out != out) return false;
		return true;
	}

	bool operator!=(const dropout_layer_t & o) const {
		return !(*this == o);
	}

	void activate(const tensor_t<float>& in ) {
		copy_input(in);
		for ( int i = 0; i < in.size.x*in.size.y*in.size.z; i++ )
		{
			bool active = (rand() % RAND_MAX) / float( RAND_MAX ) <= p_activation;
			hitmap.data[i] = active;
			out.data[i] = active ? in.data[i] : 0.0f;
		}
	}

	void fix_weights()
		{
		
		}

	void calc_grads(const tensor_t<float>& grad_next_layer )
		{
			for ( int i = 0; i < in.size.x*in.size.y*in.size.z; i++ )
				grads_in.data[i] = hitmap.data[i] ? grad_next_layer.data[i] : 0.0f;
		}
};

class opt_dropout_layer_t : public dropout_layer_t
{
public:
	opt_dropout_layer_t( tdsize in_size, float p_activation ) : dropout_layer_t(in_size, p_activation) {}
};

#ifdef INCLUDE_TESTS
namespace CNNTest{

	TEST_F(CNNTest, dropout_simple) {
		
		tdsize size(10,10,10);
		dropout_layer_t t1(size, 0.5);
		dropout_layer_t t2(size, 0.5);
		tensor_t<float> in(size);
		randomize(in);
		t1.activate(in);
		EXPECT_EQ(t1,t1);
		EXPECT_NE(t1,t2);

	}



}  // namespace
#endif

