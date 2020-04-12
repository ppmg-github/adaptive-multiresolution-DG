#include "IO.h"



IO::IO(DGSolution & dgsolution):
    dgsolution_ptr(&dgsolution), 
    max_mesh(dgsolution_ptr->nmax_level()), 
    num_grid_point(pow_int(2, max_mesh)),
    dx(1./num_grid_point),
    x_1D(num_grid_point)
{       
    for (size_t i = 0; i < num_grid_point; i++)
    {
        x_1D[i] = dx*(i+0.5);
    }
}

void IO::output_num(const std::string & file_name) const
{
    const int dim = dgsolution_ptr->DIM;

    if (dim==1)
    {
        output_num_1D(file_name);
    }
    else if (dim==2)
    {
        output_num_2D(file_name);
    }
    else if (dim==3)
    {
        output_num_3D(file_name);
    }
    else
    {
        std::cout << "dimension out of range in IO::output_num()" << std::endl; exit(1);
    }
}

void IO::output_num_cut_2D(const std::string & file_name, const double cut_x, const int cut_dim) const
{
    assert(dgsolution_ptr->DIM==3);

    std::ofstream output;
    output.open(file_name);
    const std::vector<double> y_1D = x_1D;
    std::vector<int> zero_derivative(dgsolution_ptr->DIM, 0);
    for (auto const & x : x_1D)
    {   
        for (auto const & y : y_1D)
        {
            std::vector<double> x_vec;
            if (cut_dim == 0) { x_vec = {cut_x, x, y}; }
            else if (cut_dim == 1) { x_vec = {x, cut_x, y}; }
            else if (cut_dim == 2) { x_vec = {x, y, cut_x}; }
            
            double u = dgsolution_ptr->val(x_vec, zero_derivative)[0];
            output << x << "   " << y << "   " << u << std::endl;
        }                
    }    
    output.close();
}

void IO::output_flux_lagrange(const std::string & file_name, const int unknown_var_index, const int dim, const int NMAX) const
{
    assert(dgsolution_ptr->DIM==2);
    {

    const int num_pt = pow_int(2, NMAX);
    const double h = 1./num_pt;
    std::vector<double> x_vec; 
   
    for (size_t i = 0; i < num_pt; i++)
    {
        x_vec.push_back(h*(i+0.5));
    }

        std::ofstream output;
        output.open(file_name);
        const std::vector<double> y_vec = x_vec;
        std::vector<int> zero_derivative(dgsolution_ptr->DIM, 0);
        for (auto const & x : x_vec)
        {   
            for (auto const & y : y_vec)
            {
                std::vector<double> x_vec{x, y};
                double u = dgsolution_ptr->val_Lag(x_vec, unknown_var_index, dim);
                output << x << "   " << y << "   " << u << std::endl;
            }                
        }    
        output.close();        
    }
}

void IO::output_num_exa(const std::string & file_name, std::function<double(std::vector<double>)> exact_solution) const
{
    const int dim = dgsolution_ptr->DIM;

    if (dim==1)
    {
        output_num_exa_1D(file_name, exact_solution);
    }
    else if (dim==2)
    {
        output_num_exa_2D(file_name, exact_solution);
    }
    else
    {
        std::cout << "dimension out of range in IO::output_num_exa()" << std::endl; exit(1);
    }
}

void IO::output_num_1D(const std::string & file_name) const
{
    assert(dgsolution_ptr->DIM==1);

    std::ofstream output;
    output.open(file_name);
    std::vector<int> zero_derivative(dgsolution_ptr->DIM, 0);
    for (auto const & x : x_1D)
    {   
        std::vector<double> x_vec{x};
        double u = dgsolution_ptr->val(x_vec, zero_derivative)[0];
        output << x << "   " << u << std::endl;
    }    
    output.close();
}

void IO::output_num_exa_1D(const std::string & file_name, std::function<double(std::vector<double>)> exact_solution) const
{
    assert(dgsolution_ptr->DIM==1);

    std::ofstream output;
    output.open(file_name);
    std::vector<int> zero_derivative(dgsolution_ptr->DIM, 0);
    for (auto const & x : x_1D)
    {   
        std::vector<double> x_vec{x};
        double u_num = dgsolution_ptr->val(x_vec, zero_derivative)[0];
        double u_exa = exact_solution(x_vec);
        output << x << "   " << u_num << "   " << u_exa << std::endl;
    }    
    output.close();
}

void IO::output_num_2D(const std::string & file_name) const
{
    assert(dgsolution_ptr->DIM==2);

    std::ofstream output;
    output.open(file_name);
    const std::vector<double> y_1D = x_1D;
    std::vector<int> zero_derivative(dgsolution_ptr->DIM, 0);
    for (auto const & x : x_1D)
    {   
        for (auto const & y : y_1D)
        {
            std::vector<double> x_vec{x, y};
            double u = dgsolution_ptr->val(x_vec, zero_derivative)[0];
            output << x << "   " << y << "   " << u << std::endl;
        }                
    }    
    output.close();
}

void IO::output_num_2D_controls(const std::string & file_name) const
{
	assert(dgsolution_ptr->DIM == 2);

	std::ofstream output;
	output.open(file_name);
	const std::vector<double> y_1D = x_1D;
	std::vector<int> derivative(dgsolution_ptr->DIM, 0);
	derivative[1] = 1;

	auto sign = [](double x) -> double {return ((x >= 0.) - (x < 0.)); };
	for (auto const & x : x_1D)
	{
		for (auto const & y : y_1D)
		{
			std::vector<double> x_vec{ x, y };
			double u = dgsolution_ptr->val(x_vec, derivative)[0];
			output << x << "   " << y << "   " << sign(u) << std::endl;
		}
	}
	output.close();
}

void IO::output_num_exa_2D(const std::string & file_name, std::function<double(std::vector<double>)> exact_solution) const
{
    assert(dgsolution_ptr->DIM==2);

    std::ofstream output;
    output.open(file_name);
    const std::vector<double> y_1D = x_1D;
    std::vector<int> zero_derivative(dgsolution_ptr->DIM, 0);
    for (auto const & x : x_1D)
    {   
        for (auto const & y : y_1D)
        {
            std::vector<double> x_vec{x, y};
            double u_num = dgsolution_ptr->val(x_vec, zero_derivative)[0];
            double u_exa = exact_solution(x_vec);
            output << x << "   " << y << "   " << u_num << "   " << u_exa << std::endl;
        }                
    }    
    output.close();    
}

void IO::output_num_3D(const std::string & file_name) const
{
    assert(dgsolution_ptr->DIM==3);

    std::ofstream output;
    output.open(file_name);
    const std::vector<double> y_1D = x_1D;
    const std::vector<double> z_1D = x_1D;
    std::vector<int> zero_derivative(dgsolution_ptr->DIM, 0);
    for (auto const & x : x_1D)
    {   
        for (auto const & y : y_1D)
        {
            for (auto const & z : z_1D)
            {
                std::vector<double> x_vec{x, y, z};
                double u = dgsolution_ptr->val(x_vec, zero_derivative)[0];
                output << x << "   " << y << "   " << z << "   " << u << std::endl;
            }
        }                
    }    
    output.close();
}


//
//void IO::output_num_3D_cut(const std::string & file_name, const double z) const
//{
//	assert(dgsolution_ptr->DIM == 3);
//
//	std::ofstream output;
//	output.open(file_name);
//	const std::vector<double> y_1D = x_1D;
//	const std::vector<double> z_1D = x_1D;
//	std::vector<int> zero_derivative(dgsolution_ptr->DIM, 0);
//	for (auto const & x : x_1D)
//	{
//		for (auto const & y : y_1D)
//		{
//			for (auto const & z : z_1D)
//			{
//				std::vector<double> x_vec{ x, y, z };
//				double u = dgsolution_ptr->val(x_vec, zero_derivative)[0];
//				output << x << "   " << y << "   " << z << "   " << u << std::endl;
//			}
//		}
//	}
//	output.close();
//}
//

double IO::check_symmetry(const int dim, const int num_pt)
{
    assert(dgsolution_ptr->DIM==2);
    assert(num_pt >= 1);

    double dx_small = dx/num_pt;
    int tot_pt = num_pt * num_grid_point;
    std::vector<double> x_1D_small(tot_pt);
    x_1D_small[0] = dx_small/2;
    for (size_t i = 1; i < tot_pt; i++) { x_1D_small[i] = x_1D_small[0] + i * dx_small; }

    double diff = 0;
    const std::vector<double> y_1D_small = x_1D_small;
    std::vector<int> zero_derivative(dgsolution_ptr->DIM, 0);
    for (auto const & x : x_1D_small)
    {   
        for (auto const & y : y_1D_small)
        {
            std::vector<double> x_vec{x, y};
            std::vector<double> x_sym = x_vec;
            x_sym[dim] = 1 - x_vec[dim];
            double u = dgsolution_ptr->val(x_vec, zero_derivative)[0];
            double u_sym = dgsolution_ptr->val(x_sym, zero_derivative)[0];
            diff = std::max(diff, std::abs(u-u_sym));
        }                
    }    
    return diff;
}

void IO::output_element_center(const std::string & file_name) const
{
    std::ofstream output;
    output.open(file_name);

    for (auto const & elem : dgsolution_ptr->dg)
    {
        for (size_t d = 0; d < dgsolution_ptr->DIM; d++)
        {   
            output << (elem.second.xl[d] + elem.second.xr[d])/2. << " ";
        }
        output << std::endl;
    }
    output.close();
}

void IO::output_element_support(const std::string & file_name) const
{
    std::ofstream output;
    output.open(file_name);

    for (auto const & elem : dgsolution_ptr->dg)
    {
        for (size_t d = 0; d < dgsolution_ptr->DIM; d++)
        {
            output << elem.second.xl[d] << " " <<  elem.second.xr[d] << " ";
        }
        output << std::endl;
    }
    output.close();
}

void IO::output_element_level_support(const std::string & file_name) const
{
    std::ofstream output;
    output.open(file_name);

    for (auto const & elem : dgsolution_ptr->dg)
    {
        for (size_t d = 0; d < dgsolution_ptr->DIM; d++)
        {
            output << elem.second.level[d] << " " << elem.second.xl[d] << " " <<  elem.second.xr[d] << " ";
        }
        output << std::endl;
    }
    output.close();
}

void IO::output_shock_support(const std::string & file_name) const
{
    std::ofstream output;
    output.open(file_name);

    for (auto const & elem : dgsolution_ptr->viscosity_element)
    {
        for (size_t d = 0; d < dgsolution_ptr->DIM; d++)
        {
            output << elem->xl[d] << " " <<  elem->xr[d] << " ";
        }
        output << std::endl;
    }
    output.close();
}

void IO::write_error(const int NMAX, const std::vector<double> & err, std::string file_name) const
{
    std::ofstream output;
    output.open(file_name);
    output << NMAX << " ";
    for (auto const & x : err)
    {   
        output << x << " ";
    }    
    output << std::endl;
    output.close();
}