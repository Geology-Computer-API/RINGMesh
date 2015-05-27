/*
 * Copyright (c) 2012-2015, Association Scientifique pour la Geologie et ses Applications (ASGA)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Contacts:
 *     Arnaud.Botella@univ-lorraine.fr
 *     Antoine.Mazuyer@univ-lorraine.fr
 *     Jeanne.Pellerin@wias-berlin.de
 *
 *     http://www.gocad.org
 *
 *     GOCAD Project
 *     Ecole Nationale Superieure de Geologie - Georessources
 *     2 Rue du Doyen Marcel Roubault - TSA 70605
 *     54518 VANDOEUVRE-LES-NANCY
 *     FRANCE
 */

#ifndef __RINGMESH_MACRO_MESH__
#define __RINGMESH_MACRO_MESH__

#include <ringmesh/common.h>
#include <ringmesh/utils.h>

#include <geogram/mesh/mesh.h>

namespace GEO {
    class MeshCellsAABB ;
    class MeshFacetsAABB ;
}

namespace RINGMesh {
    class BoundaryModel ;
    class MacroMesh ;
    class WellGroup ;
}

namespace RINGMesh {

    static std::vector< std::vector< vec3 > > empty_vertices ;

    /*!
     * Several modes for vertex duplication algorithm:
     *  - NONE = no duplication
     *  - FAULT = duplication along faults
     *  - HORIZON = duplication along horizons
     *  - ALL = duplication along faults and horizons
     */
    enum DuplicateMode {
        NONE, FAULT, HORIZON, ALL
    } ;
    const static index_t NB_FACET_TYPES = 2 ;
    const static index_t NB_CELL_TYPES = 4 ;

    /*!
     * Optional storage of the MacroMesh vertices
     */
    class RINGMESH_API MacroMeshVertices {
        friend class MacroMesh ;
    public:
        MacroMeshVertices( const MacroMesh& mm )
            : mm_( mm )
        {
        }

        index_t nb_vertices() const ;
        index_t vertex_id( index_t mesh, index_t v ) const ;
        const vec3& vertex( index_t global_v ) const ;
        const vec3& vertex( index_t mesh, index_t v ) const ;
        const vec3& duplicated_vertex( index_t v ) const ;

        bool vertex_id(
            index_t mesh,
            index_t cell_corner,
            index_t& vertex_id,
            index_t& duplicated_vertex_id = dummy_index_t ) const ;

        index_t nb_duplicated_vertices() const ;
        index_t nb_total_vertices() const ;

        bool is_surface_to_duplicate( index_t s ) const ;

        void clear() ;

    private:
        enum SurfaceAction {
            SKIP = -2, TO_PROCESS = -1, NEG_SIDE = 0, POS_SIDE = 1
        } ;
        typedef std::pair< index_t, SurfaceAction > surface_side ;

        void initialize() ;
        void initialize_duplication() ;

        bool duplicate_corner(
            const std::set< surface_side >& surfaces,
            std::vector< SurfaceAction >& info ) ;

    private:
        /// Attached MaroMesh
        const MacroMesh& mm_ ;

        /// Vector of the vertices with different coordinates
        std::vector< vec3 > unique_vertices_ ;
        /// Mapping between the vertex ids in a GEO::Mesh and a MacroMesh
        std::vector< index_t > global_vertex_indices_ ;
        /// Mapping between the mesh id and the global_vertex_indices_ vector
        std::vector< index_t > vertex2mesh_ ;

        /// Vector of the cell corners, stores the vertex id (can duplicated)
        std::vector< index_t > cell_corners_ ;
        /// Mapping between the mesh id and the cell_corners_ vector
        std::vector< index_t > mesh_cell_corner_ptr_ ;
        /// Mapping between the duplicated vertices and the unique vertex ids
        std::vector< index_t > duplicated_vertex_indices_ ;
    } ;

    /*!
     * Optional storage of the MacroMesh facets
     */
    class RINGMESH_API MacroMeshFacets {
    public:
        MacroMeshFacets( const MacroMesh& mm )
            : mm_( mm ), nb_facets_( 0 ), nb_triangle_( 0 ), nb_quad_( 0 )
        {
        }

        index_t facet( index_t s, index_t f ) const ;
        index_t mesh( index_t s ) const ;
        index_t nb_facets() const ;
        index_t nb_facets( index_t s ) const ;

        index_t nb_triangle() const ;
        index_t nb_triangle( index_t s ) const ;
        index_t triangle_id( index_t s, index_t t ) const ;

        index_t nb_quad() const ;
        index_t nb_quad( index_t s ) const ;
        index_t quad_id( index_t s, index_t q ) const ;

        void clear() ;

    private:
        /*!
         * Id where to start reading the vector surface_facets_ for a given surface
         * @param[in] s id of the surface
         * @return the corresponding id
         */
        index_t surface_begin( index_t s ) const
        {
            return surface_facet_ptr_[NB_FACET_TYPES * s] ;
        }
        /*!
         * Id where to stop reading the vector surface_facets_ for a given surface
         * @param[in] s id of the surface
         * @return the corresponding id
         */
        index_t surface_end( index_t s ) const
        {
            return surface_facet_ptr_[NB_FACET_TYPES * ( s + 1 )] ;
        }
        /*!
         * Accessor for the surface_facets_ vector
         * @param[in] global_f the id to read
         * @return the value in the vector
         */
        index_t facet( index_t global_f ) const
        {
            return surface_facets_[global_f] ;
        }

        void initialize() ;

    private:
        /// Attached MaroMesh
        const MacroMesh& mm_ ;

        /// Vector of the facet ids in the corresponding GEO::Mesh
        std::vector< index_t > surface_facets_ ;
        /// Mapping between surface id and facet elements in surface_facets_
        std::vector< index_t > surface_facet_ptr_ ;
        /// Mapping between the surface id and the GEO::Mesh
        std::vector< index_t > surface2mesh_ ;

        /// Number of facets in the MacroMesh
        index_t nb_facets_ ;
        /// Number of triangles in the MacroMesh
        index_t nb_triangle_ ;
        /// Number of quad in the MacroMesh
        index_t nb_quad_ ;
    } ;

    /*!
     * Optional storage of the MacroMesh cells
     */
    class RINGMESH_API MacroMeshCells {
    public:
        MacroMeshCells( const MacroMesh& mm )
            :
                mm_( mm ),
                nb_cells_( 0 ),
                nb_tet_( 0 ),
                nb_pyramid_( 0 ),
                nb_prism_( 0 ),
                nb_hex_( 0 )
        {
        }

        index_t cell_adjacent( index_t mesh, index_t c, index_t f ) const ;

        index_t cell_index_in_mesh( index_t global_index, index_t& mesh_id ) const ;

        index_t nb_cells() const ;
        index_t nb_cells( index_t r ) const ;

        index_t nb_tet() const ;
        index_t nb_tet( index_t r ) const ;
        index_t tet_id( index_t r, index_t t ) const ;

        index_t nb_pyramid() const ;
        index_t nb_pyramid( index_t r ) const ;
        index_t pyramid_id( index_t r, index_t p ) const ;

        index_t nb_prism() const ;
        index_t nb_prism( index_t r ) const ;
        index_t prism_id( index_t r, index_t p ) const ;

        index_t nb_hex() const ;
        index_t nb_hex( index_t r ) const ;
        index_t hex_id( index_t r, index_t h ) const ;

        void clear() ;

    private:
        /*!
         * Tests if the MacroMeshCells needs to be initialized and initialize it
         */
        void test_initialize() const
        {
            if( nb_cells_ == 0 ) {
                const_cast< MacroMeshCells* >( this )->initialize() ;
            }
        }
        void initialize() ;
        /*!
         * Id where to start reading the vector mesh_cell_ptr_ for a given mesh
         * @param[in] mesh id of the mesh
         * @return the corresponding id
         */
        index_t mesh_begin( index_t mesh ) const
        {
            return mesh_cell_ptr_[NB_CELL_TYPES * mesh] ;
        }
        /*!
         * Id where to stop reading the vector mesh_cell_ptr_ for a given mesh
         * @param[in] mesh id of the mesh
         * @return the corresponding id
         */
        index_t mesh_end( index_t mesh ) const
        {
            return mesh_cell_ptr_[NB_CELL_TYPES * mesh] ;
        }

    private:
        /// Attached MaroMesh
        const MacroMesh& mm_ ;

        /// Vector of the cell ids in the corresponding GEO::Mesh
        std::vector< index_t > cells_ ;
        /// Vector of the adjacent cell ids in the MacroMesh
        std::vector< index_t > cell_adjacents_ ;
        /// Mapping between mesh id and cell elements in cells_
        std::vector< index_t > mesh_cell_ptr_ ;

        /// Number of cells in the MacroMesh
        index_t nb_cells_ ;
        /// Number of tetrahedra in the MacroMesh
        index_t nb_tet_ ;
        /// Number of pyramids in the MacroMesh
        index_t nb_pyramid_ ;
        /// Number of prisms in the MacroMesh
        index_t nb_prism_ ;
        /// Number of hexahedra in the MacroMesh
        index_t nb_hex_ ;
    } ;

    /*!
     * Optional storage of the MacroMesh tools
     */
    class RINGMESH_API MacroMeshTools {
    public:
        MacroMeshTools( MacroMesh& mm ) ;
        ~MacroMeshTools() ;

        const GEO::MeshFacetsAABB& facet_aabb( index_t region ) const ;
        const GEO::MeshCellsAABB& cell_aabb( index_t region ) const ;

        void clear() ;

    private:
        void init_facet_aabb( index_t region ) const ;
        void init_cell_aabb( index_t region ) const ;

    private:
        /// Attached MaroMesh
        MacroMesh& mm_ ;

        /// Storage of the AABB trees on the facets
        std::vector< GEO::MeshFacetsAABB* > facet_aabb_ ;
        /// Storage of the AABB trees on the cells
        std::vector< GEO::MeshCellsAABB* > cell_aabb_ ;
    } ;

    class RINGMESH_API MacroMeshOrder {
    public:
        MacroMeshOrder( MacroMesh& mm ) ;
        ~MacroMeshOrder() ;
        const index_t nb_total_vertices() const ;
        const index_t id(const vec3& point) const ;
        void clear() ;
    private:
        void initialize() ;
        /*!
         * Tests if the MacroMeshOrder needs to be initialized and initialize it
         */
        void test_initialize() const
        {
            if( nb_vertices_ == 0 ) {
                const_cast< MacroMeshOrder* >( this )->initialize() ;
            }
        }
    private:
        /// Attached MaroMesh
        const MacroMesh& mm_ ;
        /// Total number of vertices + new nodes on cell edges
        index_t nb_vertices_ ;
        /// ANNTree composed only with new nodes on cell edges
        ColocaterANN ann_ ;

    } ;

    class RINGMESH_API MacroMesh {
    ringmesh_disable_copy( MacroMesh ) ;
    public:

        MacroMesh( const BoundaryModel& model ) ;
        MacroMesh() ;
        virtual ~MacroMesh() ;

        //    __  __     _   _            _
        //   |  \/  |___| |_| |_  ___  __| |___
        //   | |\/| / -_)  _| ' \/ _ \/ _` (_-<
        //   |_|  |_\___|\__|_||_\___/\__,_/__/
        //
        void compute_tetmesh(
            const TetraMethod& method,
            int region_id = -1,
            bool add_steiner_points = true,
            std::vector< std::vector< vec3 > >& internal_vertices = empty_vertices ) ;
        void copy( const MacroMesh& mm, bool copy_attributes = true ) const ;

        //      _
        //     /_\  __ __ ___ _________ _ _ ___
        //    / _ \/ _/ _/ -_|_-<_-< _ \ '_(_-<
        //   /_/ \_\__\__\___/__/__|___/_| /__/
        //
        /*!
         * Access to a GEO::Mesh of the MacroMesh
         * @param[in] region id of mesh/region
         * @return a reference to the GEO::Mesh
         */
        GEO::Mesh& mesh( index_t region ) const
        {
            return *meshes_[region] ;
        }

        /*!
         * Get the number of meshes/regions
         * @return the corresponding number
         */
        index_t nb_meshes() const
        {
            return meshes_.size() ;
        }

        void add_wells( const WellGroup* wells ) ;
        const WellGroup* wells() const
        {
            return wells_ ;
        }

        /*!
         * Access to the BoundaryModel attached to the MacroMesh
         * @return a const reference to the corresponding BoundaryModel
         */
        const BoundaryModel& model() const
        {
            ringmesh_debug_assert( model_ ) ;
            return *model_ ;
        }
        void set_nodel( const BoundaryModel& model ) ;

        /*!
         * Access the DuplicateMode
         * @return the current DuplicateMode
         */
        DuplicateMode duplicate_mode() const
        {
            return mode_ ;
        }
        /*!
         * Set a new DuplicateMode
         * @param[in] mode the new DuplicateMode for the MacroMesh
         */
        void set_duplicate_mode( const DuplicateMode& mode ) const
        {
            MacroMesh* not_const = const_cast< MacroMesh* >( this ) ;
            not_const->mode_ = mode ;
            not_const->vertices.cell_corners_.clear() ;
            not_const->vertices.duplicated_vertex_indices_.clear() ;
            not_const->vertices.mesh_cell_corner_ptr_.clear() ;
        }

        /*
         * Change the order of the mesh
         */
        void set_order(const index_t o) {
            if(o != order_) {
                order.clear() ;
            }
            order_ = o ;
        }

        /*
         * Gets the mesh elements order
         * @return the const order
         */
        const index_t get_order() const {
            return order_ ;
        }

    protected:
        /// BoundaryModel representing the structural information of the mesh
        const BoundaryModel* model_ ;
        /// Vector of meshes, one by region
        std::vector< GEO::Mesh* > meshes_ ;
        /// Optional duplication mode to compute the duplication of vertices on surfaces
        DuplicateMode mode_ ;

        /// Optional WellGroup associated with the model
        const WellGroup* wells_ ;

        /// Order of the mesh
        index_t order_ ;

    public:
        /// Optional storage of the MacroMesh vertices
        MacroMeshVertices vertices ;
        /// Optional storage of the MacroMesh facets
        MacroMeshFacets facets ;
        /// Optional storage of the MacroMesh cells
        MacroMeshCells cells ;
        /// Optional storage of tools
        MacroMeshTools tools ;
        /// Optional storage for high orders mesh
        MacroMeshOrder order ;


    } ;
}

#endif