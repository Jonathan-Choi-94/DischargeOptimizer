#pragma once
#include "UdeOptBase.h"

//Nelder-Mead algorithm
class UDEOPT_EXT_CLASS NmSimplex : public UdeOptBase
{
public:
	NmSimplex(const double & scale, const double & epsilon, const int & max_iteration );
	virtual ~NmSimplex();
	bool Run() override;

private:
	/*APIs for printing the optimization status */
	//execute the pre-process for NM simplex search 
	bool Initialize();
	//Two log APIs
	void PrintSimplex() const;
	//Two log APIs
	void LogSimplex(const int& iteration_count) const;

	/*APIs for NM simplex method */
	//Initialize the num_opt_states+1 array variables for NM Simplex  
	void MakeInitSimplex();
	//allocate the arrays  
	void AllocNmArray();
	//get averaged points (centroid)
	void UpdateCentroid(const int & vg) const;
	//exec. shrink 
	void DoShrink(double** v, const int& vs) const;

	//accept the new candidate
	void AcceptNewSimplex(const int& lid, double* src_candidate, const double& err) ;
	//sort the vector of Id-Error pair
	bool RenewIndexErrPairVec();

	//parametesr to decide space between initial candidates
	double scale_;
	//accepted error margin to terminate the iterative optimization
	double err_epsilon_;
	//assigned maximum iteration count for searching optimal point
	int max_iteration_;

public:
	double** v_; /* holds vertices of simplex */
	double* vr_; /* reflection - coordinates */
	double* ve_; /* expansion - coordinates */
	double* vc_; /* contraction - coordinates */
	double* vm_; /* centroid - coordinates */
	double* f_; /* value of function at each vertex */

	double err_previous, err_current;
	int final_count;

	//Return the final parameters
	std::vector<double> GetFinalParams();
	int index_min;
	
	std::vector<std::pair<int, double> > sorted_id_err_pair_vec;
};

