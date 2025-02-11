#pragma once
#include "DGSolution.h"


// DGAapt is derived from DGSolution
class DGAdapt :
	public DGSolution
{
public:
	DGAdapt(const bool sparse_, const int level_init_, const int NMAX_, AllBasis<AlptBasis> & all_bas_, AllBasis<LagrBasis> & all_bas_Lag_, AllBasis<HermBasis> & all_bas_Her_, Hash & hash_, const double eps_, const double eta_, const bool is_find_ptr_alpt_, const bool is_find_ptr_intp_, const bool is_find_ptr_general_ = false);
	DGAdapt(const bool sparse_, const int level_init_, const int NMAX_, const int auxiliary_dim_, AllBasis<AlptBasis> & all_bas_, AllBasis<LagrBasis> & all_bas_Lag_, AllBasis<HermBasis> & all_bas_Her_, Hash & hash_, const double eps_, const double eta_, const bool is_find_ptr_alpt_, const bool is_find_ptr_intp_, const bool is_find_ptr_general_ = false);
	~DGAdapt() {};

	/// adaptive initialization for seperable initial value
	/// a function of separable form f(x1,x2,...,xdim) = g_1(x1) * g_2(x2) * ... * g_dim(xdim)
	/// input is a functional: (x, d) -> g_d(x)
	void init_separable_scalar(std::function<double(double, int)> scalar_func);
	void init_separable_system(std::vector<std::function<double(double, int)>> vector_func);
	
	/**
	 * @brief adaptive initialization a function to be a summation of separable scalar functions, i.e. \n 
	 * 	f(x1, x2, ..., xdim) = \sum_(i=1)^(num_func) f_i(x1, x2, ..., xdim) \n 
	 * 	with each f_i to be a function of separable form \n 
	 *	f_i(x1, x2, ..., xdim) = g_1(x1) * g_2(x2) * ... * g_dim(xdim)
	 * 
	 * @param sum_scalar_func function to be projected, int num_separable_func = sum_func.size()
	 *  
	 * @note 	If the initial condition is \n
	 * f(x,y) = sin(2*pi*(x+y)) = sin(2*pi*x) * cos(2*pi*y) + cos(2*pi*x) * sin(2*pi*y) \n 
	 * then example is \n 
	 * auto init_func_1 = [](double x, int d) { return (d==0) ? (sin(2*pi*x)) : (cos(2*pi*x)); }; \n
	 * auto init_func_2 = [](double x, int d) { return (d==0) ? (cos(2*pi*x)) : (sin(2*pi*x)); }; \n 
	 * std::vector<std::function<double(double, int)>> init_func{init_func_1, init_func_2}; \n 
	 * dg_solu.init_separable_scalar_sum(init_func);
	 */
	void init_separable_scalar_sum(std::vector<std::function<double(double, int)>> sum_scalar_func);
	void init_separable_system_sum(std::vector<std::vector<std::function<double(double, int)>>> sum_vector_func);

	/**
	 * @brief compute moment of a distribution (scalar) function f = f(x, v),
	 * 		  then ADD it to Element::rhs of the current dg solution in the num_vec component
	 * 
	 * @param f 
	 * @param moment_order 
	 * @param moment_order_weight 
	 * @param num_vec 
	 */
	void compute_moment_full_grid(DGAdapt & f, const std::vector<int> & moment_order, const std::vector<double> & moment_order_weight, const int num_vec = 0);

	/**
	 * @brief compute moment of a distribution (scalar) function f = f(x2, v1, v2),
	 * 		  then ADD it to Element::rhs of the current dg solution in the num_vec component
	 * 
	 * @param f 
	 * @param moment_order = {moment_order_v1, moment_order_v2}
	 * @param moment_order_weight 
	 * @param num_vec_EB 
	 * @param num_vec_f 
	 */
	void compute_moment_1D2V(DGAdapt & f, const std::vector<int> & moment_order, const double moment_order_weight, const int num_vec_EB, const int num_vec_f);

	void compute_moment_2D2V(DGAdapt & f, const std::vector<int> & moment_order, const double moment_order_weight, const int num_vec_EB, const int num_vec_f);
	
	// adaptive add elements in f for all the elements in E
	void adapt_f_base_on_E(DGAdapt & E);
	
	// refine based on reference solution generated by Euler forward
	void refine();

	// refine to some max mesh level in given dimension
	// NOT TESTED
	void refine(const int max_mesh, const std::vector<int> dim);

	// coarsen
	void coarsen();

	// coarsen to some max mesh level in given dimension
	// NOT TESTED
	void coarsen(const int max_mesh, const std::vector<int> dim);

	// update elements with artificial viscosity
	void update_viscosity_element(const double shock_kappa);

	static std::vector<int> indicator_var_adapt;

protected:
	const double eps;
	const double eta;

	std::unordered_map<int, Element*> leaf;		// leaf (element which do not have its all children in DGSolution)
	std::unordered_map<int, Element*> leaf_zero_child;	// leaf (element which have zero children in DGSolution)

	// set new_add variable in all elements to be false
	void set_all_new_add_false();

	// after adding or deleting, check no holes in solution
	void check_hole();

	// update leaf
	// if num of existing children is not the same as num of total children, then add it to leaf element
	void update_leaf();


	// add a given element into DG solution
	void add_elem(Element & elem);
	
	// delete a given element into DG solution
	void del_elem(Element & elem);

	// return collections of index of parent/children of given index
	std::set<std::array<std::vector<int>,2>> index_all_par( const std::vector<int> & lev_n, const std::vector<int> & sup_j);
	std::set<std::array<std::vector<int>,2>> index_all_chd( const std::vector<int> & lev_n, const std::vector<int> & sup_j);

private:
	
	const bool is_find_ptr_alpt;
	const bool is_find_ptr_intp;
	const bool is_find_ptr_general;

	// check that total num of children and parents in all elements are equal
	// this function will only be used for debug
	bool check_total_num_chd_par_equal() const;

	// initial number and pointers of parents and children
	void init_chd_par();

	// recursively refine for seperable initial value
	void refine_init_separable_scalar(const VecMultiD<double> & coeff);
	void refine_init_separable_system(const std::vector<VecMultiD<double>> & coeff);

	// recursively refine for summation of seperable initial value 
	void refine_init_separable_scalar_sum(const std::vector<VecMultiD<double>> & coeff);
	void refine_init_separable_system_sum(const std::vector<std::vector<VecMultiD<double>>> & coeff);

	// coarsen based on leaf with 0 child
	void coarsen_no_leaf();

	// update leaf with 0 child
	// if num of existing children is not the same as num of total children, then add it to leaf element
	void update_leaf_zero_child();

	void check_hole_init_separable_scalar(const VecMultiD<double> & coeff);
	void check_hole_init_separable_system(const std::vector<VecMultiD<double>> & coeff);

	void check_hole_init_separable_scalar_sum(const std::vector<VecMultiD<double>> & coeff);
	void check_hole_init_separable_system_sum(const std::vector<std::vector<VecMultiD<double>>> & coeff);

	// return L2 norm of functions in an element
 	double indicator_norm(const Element & elem) const;

	// return number of all parents (no matter if already in dg) of element with given mesh level n
	int num_all_par(const std::vector<int> & lev_n) const;

	// return number of all children (no matter if already in dg) of element with given mesh level n
	int num_all_chd(const std::vector<int> & lev_n) const;
};

