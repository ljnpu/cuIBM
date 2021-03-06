/**
 * \file updateRHS1.inl
 * \brief Implementation of the methods of the class \c DirectForcingSolver to update
 *        the right hand-side of the system for the intermediate velocity flux vector.
 */


#include <solvers/kernels/updateRHS1.h>


/**
 * \brief Update the RHS of the velocity system.
 *
 * The vector \a rhs1 is first set up as if it would have been in the 
 * absence of an immersed boundary. This function then changes values 
 * in the vector that correspond to the forcing nodes on the grid, 
 * replacing them with the rhs values from the interpolation relations.
 */
template <>
void DirectForcingSolver<device_memory>::updateRHS1()
{
	int nx = domInfo->nx,
	    ny = domInfo->ny;
	int numUV = (nx-1)*ny + nx*(ny-1);
	
	real dt = (*paramDB)["simulation"]["dt"].get<real>();
	
	real *rhs1_r  = thrust::raw_pointer_cast(&(rhs1[0]));
	int *tags_r  = thrust::raw_pointer_cast(&(tagsD[0])),
	    *tags2_r = thrust::raw_pointer_cast(&(tags2D[0]));

	real *dx_r = thrust::raw_pointer_cast( &(domInfo->dxD[0]) ),
	     *dy_r = thrust::raw_pointer_cast( &(domInfo->dyD[0]) ),
	     *coeffs_r = thrust::raw_pointer_cast( &(coeffsD[0]) ),
	     *coeffs2_r = thrust::raw_pointer_cast( &(coeffs2D[0]) ),
	     *uv_r = thrust::raw_pointer_cast( &(uvD[0]) );

	const int blocksize = 256;
	dim3 dimGrid( int( ((nx-1)*ny+nx*(ny-1)-0.5)/blocksize ) + 1, 1);
	dim3 dimBlock(blocksize, 1);
	
	// 1-d interpolation
	kernels::updateRHS1 <<<dimGrid, dimBlock>>> (rhs1_r, numUV, tags_r);
	
	//kernels::updateRHS1X <<<dimGrid, dimBlock>>>(rhs1_r, nx, ny, dt, dx_r, tags_r, coeffs_r, uv_r);
	//kernels::updateRHS1Y <<<dimGrid, dimBlock>>>(rhs1_r, nx, ny, dt, dy_r, tags_r, coeffs_r, uv_r);
	kernels::updateRHS1X <<<dimGrid, dimBlock>>>(rhs1_r, nx, ny, dt, dx_r, tags_r, coeffs_r, coeffs2_r, uv_r);
	kernels::updateRHS1Y <<<dimGrid, dimBlock>>>(rhs1_r, nx, ny, dt, dy_r, tags_r, coeffs_r, coeffs2_r, uv_r);
} // updateRHS1
