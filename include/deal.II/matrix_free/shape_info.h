// ---------------------------------------------------------------------
//
// Copyright (C) 2011 - 2017 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE at
// the top level of the deal.II distribution.
//
// ---------------------------------------------------------------------


#ifndef dealii__matrix_free_shape_info_h
#define dealii__matrix_free_shape_info_h


#include <deal.II/base/exceptions.h>
#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/vectorization.h>
#include <deal.II/base/aligned_vector.h>
#include <deal.II/fe/fe.h>

#include <deal.II/matrix_free/helper_functions.h>


DEAL_II_NAMESPACE_OPEN

namespace internal
{
  namespace MatrixFreeFunctions
  {
    /**
     * An enum that encodes the type of element detected during
     * initialization. FEEvaluation will select the most efficient algorithm
     * based on the given element type.
     */
    enum ElementType
    {
      /**
       * Tensor product shape function where the shape value evaluation in the
       * quadrature point corresponds to the identity operation and no
       * interpolation needs to be performed (collocation approach, also
       * called spectral evaluation). This is for example the case for an
       * element with nodes in the Gauss-Lobatto support points and
       * integration in the Gauss-Lobatto quadrature points of the same order.
       */
      tensor_symmetric_collocation = 0,
      /**
       * Symmetric tensor product shape functions fulfilling a Hermite
       * identity with values and first derivatives zero at the element end
       * points in 1D.
       */
      tensor_symmetric_hermite = 1,
      /**
       * Usual tensor product shape functions whose shape values and
       * quadrature points are symmetric about the midpoint of the unit
       * interval 0.5
       */
      tensor_symmetric = 2,
      /**
       * Tensor product shape functions without further particular properties
       */
      tensor_general = 3,
      /**
       * Polynomials of complete degree rather than tensor degree which can be
       * described by a truncated tensor product
       */
      truncated_tensor = 4,
      /**
       * Tensor product shape functions that are symmetric about the midpoint
       * of the unit interval 0.5 that additionally add a constant shape
       * function according to FE_Q_DG0.
       */
      tensor_symmetric_plus_dg0 = 5
    };

    /**
     * The class that stores the shape functions, gradients and Hessians
     * evaluated for a tensor product finite element and tensor product
     * quadrature formula on the unit cell. Because of this structure, only
     * one-dimensional data is stored.
     *
     * @author Katharina Kormann and Martin Kronbichler, 2010, 2011
     */
    template <typename Number>
    struct ShapeInfo
    {
      /**
       * Empty constructor. Does nothing.
       */
      ShapeInfo ();

      /**
       * Constructor that initializes the data fields using the reinit method.
       */
      template <int dim>
      ShapeInfo (const Quadrature<1> &quad,
                 const FiniteElement<dim> &fe,
                 const unsigned int base_element = 0);

      /**
       * Initializes the data fields. Takes a one-dimensional quadrature
       * formula and a finite element as arguments and evaluates the shape
       * functions, gradients and Hessians on the one-dimensional unit cell.
       * This function assumes that the finite element is derived from a one-
       * dimensional element by a tensor product and that the zeroth shape
       * function in zero evaluates to one.
       */
      template <int dim>
      void reinit (const Quadrature<1> &quad,
                   const FiniteElement<dim> &fe_dim,
                   const unsigned int base_element = 0);

      /**
       * Return the memory consumption of this class in bytes.
       */
      std::size_t memory_consumption () const;

      /**
       * Encodes the type of element detected at construction. FEEvaluation
       * will select the most efficient algorithm based on the given element
       * type.
       */
      ElementType element_type;

      /**
       * Stores the shape values of the 1D finite element evaluated on all 1D
       * quadrature points in vectorized format, i.e., as an array of
       * VectorizedArray<dim>::n_array_elements equal elements. The length of
       * this array is <tt>n_dofs_1d * n_q_points_1d</tt> and quadrature
       * points are the index running fastest.
       */
      AlignedVector<VectorizedArray<Number> > shape_values;

      /**
       * Stores the shape gradients of the 1D finite element evaluated on all
       * 1D quadrature points in vectorized format, i.e., as an array of
       * VectorizedArray<dim>::n_array_elements equal elements. The length of
       * this array is <tt>n_dofs_1d * n_q_points_1d</tt> and quadrature
       * points are the index running fastest.
       */
      AlignedVector<VectorizedArray<Number> > shape_gradients;

      /**
       * Stores the shape Hessians of the 1D finite element evaluated on all
       * 1D quadrature points in vectorized format, i.e., as an array of
       * VectorizedArray<dim>::n_array_elements equal elements. The length of
       * this array is <tt>n_dofs_1d * n_q_points_1d</tt> and quadrature
       * points are the index running fastest.
       */
      AlignedVector<VectorizedArray<Number> > shape_hessians;

      /**
       * Stores the shape values in a different format, namely the so-called
       * even-odd scheme where the symmetries in shape_values are used for
       * faster evaluation.
       */
      AlignedVector<VectorizedArray<Number> > shape_values_eo;

      /**
       * Stores the shape gradients in a different format, namely the so-
       * called even-odd scheme where the symmetries in shape_gradients are
       * used for faster evaluation.
       */
      AlignedVector<VectorizedArray<Number> > shape_gradients_eo;

      /**
       * Stores the shape second derivatives in a different format, namely the
       * so-called even-odd scheme where the symmetries in shape_hessians are
       * used for faster evaluation.
       */
      AlignedVector<VectorizedArray<Number> > shape_hessians_eo;

      /**
       * Stores the shape gradients of the shape function space associated to
       * the quadrature (collocation), given by FE_DGQ<1>(Quadrature<1>). For
       * faster evaluation only the even-odd format is necessary.
       */
      AlignedVector<VectorizedArray<Number> > shape_gradients_collocation_eo;

      /**
       * Stores the shape hessians of the shape function space associated to
       * the quadrature (collocation), given by FE_DGQ<1>(Quadrature<1>). For
       * faster evaluation only the even-odd format is necessary.
       */
      AlignedVector<VectorizedArray<Number> > shape_hessians_collocation_eo;

      /**
       * Stores the indices from cell DoFs to face DoFs. The rows go through
       * the <tt>2*dim</tt> faces, and the columns the DoFs on the faces.
       */
      dealii::Table<2,unsigned int>  face_indices;

      /**
       * Stores one-dimensional values of shape functions evaluated in zero
       * and one, i.e., on the one-dimensional faces. Not vectorized.
       */
      std::vector<Number>    face_value[2];

      /**
       * Stores one-dimensional gradients of shape functions evaluated in zero
       * and one, i.e., on the one-dimensional faces. Not vectorized.
       */
      std::vector<Number>    face_gradient[2];

      /**
       * Stores one-dimensional values of shape functions on subface. Since
       * there are two subfaces, store two variants. Not vectorized.
       */
      std::vector<Number>    subface_value[2];

      /**
       * Non-vectorized version of shape values. Needed when evaluating face
       * info.
       */
      std::vector<Number>    shape_values_number;

      /**
       * Non-vectorized version of shape gradients. Needed when evaluating
       * face info.
       */
      std::vector<Number>    shape_gradient_number;

      /**
       * Renumbering from deal.II's numbering of cell degrees of freedom to
       * lexicographic numbering used inside the FEEvaluation schemes of the
       * underlying element in the DoFHandler. For vector-valued elements, the
       * renumbering starts with a lexicographic numbering of the first
       * component, then everything of the second component, and so on.
       */
      std::vector<unsigned int> lexicographic_numbering;

      /**
       * Stores the degree of the element.
       */
      unsigned int fe_degree;

      /**
       * Stores the number of quadrature points per dimension.
       */
      unsigned int n_q_points_1d;

      /**
       * Stores the number of quadrature points in @p dim dimensions for a
       * cell.
       */
      unsigned int n_q_points;

      /**
       * Stores the number of DoFs per cell in @p dim dimensions.
       */
      unsigned int dofs_per_cell;

      /**
       * Stores the number of quadrature points per face in @p dim dimensions.
       */
      unsigned int n_q_points_face;

      /**
       * Stores the number of DoFs per face in @p dim dimensions.
       */
      unsigned int dofs_per_face;

      /**
       * Check whether we have symmetries in the shape values. In that case,
       * also fill the shape_???_eo fields.
       */
      bool check_1d_shapes_symmetric(const unsigned int n_q_points_1d);

      /**
       * Check whether symmetric 1D basis functions are such that the shape
       * values form a diagonal matrix, i.e., the nodal points are collocated
       * with the quadrature points. This allows for specialized algorithms
       * that save some operations in the evaluation.
       */
      bool check_1d_shapes_collocation();
    };



    // ------------------------------------------ inline functions

    template <typename Number>
    template <int dim>
    inline
    ShapeInfo<Number>::ShapeInfo (const Quadrature<1> &quad,
                                  const FiniteElement<dim> &fe_in,
                                  const unsigned int base_element_number)
      :
      element_type(tensor_general),
      fe_degree (0),
      n_q_points_1d (0),
      n_q_points (0),
      dofs_per_cell (0),
      n_q_points_face (0),
      dofs_per_face (0)
    {
      reinit (quad, fe_in, base_element_number);
    }

  } // end of namespace MatrixFreeFunctions

} // end of namespace internal

DEAL_II_NAMESPACE_CLOSE

#endif
