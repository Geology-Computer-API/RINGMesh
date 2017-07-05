/*
 * Copyright (c) 2012-2017, Association Scientifique pour la Geologie et ses Applications (ASGA)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of ASGA nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ASGA BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *     http://www.ring-team.org
 *
 *     RING Project
 *     Ecole Nationale Superieure de Geologie - GeoRessources
 *     2 Rue du Doyen Marcel Roubault - TSA 70605
 *     54518 VANDOEUVRE-LES-NANCY
 *     FRANCE
 */

#include <ringmesh/geomodel/geomodel_api.h>

#include <iomanip>
#include <iostream>

#include <geogram/basic/progress.h>

#include <ringmesh/basic/geometry.h>

#include <ringmesh/geomodel/geomodel.h>
#include <ringmesh/geomodel/geomodel_entity.h>
#include <ringmesh/geomodel/geomodel_mesh_entity.h>
#include <ringmesh/geomodel/geomodel_geological_entity.h>

#include <ringmesh/tetrahedralize/tetra_gen.h>

/*!
 * @file Set of high level API functions
 */

namespace {
    using namespace RINGMesh;

    /*!
     * @brief Total number of polygons in the geomodel Surfaces
     */
    index_t count_geomodel_polygons( const GeoModel& geomodel )
    {
        index_t result = 0;
        for( index_t i = 0; i < geomodel.nb_surfaces(); ++i ) {
            result += geomodel.surface( i ).nb_mesh_elements();
        }
        return result;
    }

    index_t count_geomodel_cells( const GeoModel& geomodel )
    {
        index_t nb_cells = 0;
        for( index_t i = 0; i < geomodel.nb_regions(); ++i ) {
            nb_cells += geomodel.region( i ).nb_mesh_elements();
        }
        return nb_cells;
    }

    index_t count_geomodel_edges( const GeoModel& geomodel )
    {
        index_t nb_edges = 0;
        for( index_t i = 0; i < geomodel.nb_lines(); ++i ) {
            nb_edges += geomodel.line( i ).nb_mesh_elements();
        }
        return nb_edges;
    }

    void compute_region_volumes_per_cell_type(
        const Region& region,
        double& tet_volume,
        double& pyramid_volume,
        double& prism_volume,
        double& hex_volume,
        double& poly_volume )
    {
        region.compute_region_volumes_per_cell_type( tet_volume, pyramid_volume,
            prism_volume, hex_volume, poly_volume );

    }

    double compute_geomodel_volumes_per_cell_type(
        const GeoModel& geomodel,
        double& tet_volume,
        double& pyramid_volume,
        double& prism_volume,
        double& hex_volume,
        double& poly_volume )
    {
        tet_volume = 0;
        pyramid_volume = 0;
        prism_volume = 0;
        hex_volume = 0;
        poly_volume = 0;
        for( index_t r = 0; r < geomodel.nb_regions(); r++ ) {
            const Region& region = geomodel.region( r );
            compute_region_volumes_per_cell_type( region, tet_volume, pyramid_volume,
                prism_volume, hex_volume, poly_volume );
        }
        return tet_volume + pyramid_volume + prism_volume + hex_volume + poly_volume;
    }

    void print_one_cell_stat(
        const index_t& nb_cell,
        const index_t& nb_cell_total,
        const std::string& cell_type )
    {
        Logger::out( "GeoModel", "* ", nb_cell, " ", cell_type, " (",
            nb_cell * 100 / nb_cell_total, "%)" );
    }

    void print_one_cell_volume_stat(
        const double& cell_volume,
        const double& cell_volume_total,
        const std::string& cell_type )
    {
        Logger::out( "GeoModel", "* ", cell_type, " volume ", cell_volume, " (",
            static_cast< index_t >( cell_volume * 100 / cell_volume_total + 0.5 ),
            "%)" );
    }
}

namespace RINGMesh {

    void print_nb_mesh_entities(
        const GeoModel& geomodel,
        const MeshEntityType& type )
    {
        Logger::out( "GeoModel", std::setw( 10 ), std::left,
            geomodel.nb_mesh_entities( type ), " ", type );
    }

    void print_nb_geological_entities(
        const GeoModel& geomodel,
        const GeologicalEntityType& type )
    {
        if( geomodel.nb_geological_entities( type ) == 0 ) {
            return;
        }
        Logger::out( "GeoModel", std::setw( 10 ), std::left,
            geomodel.nb_geological_entities( type ), " ", type );
    }

    void print_geomodel( const GeoModel& geomodel )
    {
        Logger::out( "GeoModel", "Model ", geomodel.name(), " has\n",
            std::setw( 10 ), std::left, geomodel.mesh.vertices.nb(), " vertices\n",
            std::setw( 10 ), std::left, count_geomodel_polygons( geomodel ),
            " polygons" );
        index_t nb_cells = count_geomodel_cells( geomodel );
        if( nb_cells != 0 ) {
            Logger::out( "GeoModel", std::setw( 10 ), std::left, nb_cells,
                " cells" );
        }
        Logger::out( "GeoModel" );

        const EntityTypeManager& manager = geomodel.entity_type_manager();
        const std::vector< MeshEntityType >& mesh_entity_types =
            manager.mesh_entity_manager.mesh_entity_types();
        for( const MeshEntityType& type : mesh_entity_types ) {
            print_nb_mesh_entities( geomodel, type );
        }
        const std::vector< GeologicalEntityType >& geological_entity_types =
            manager.geological_entity_manager.geological_entity_types();
        for( const GeologicalEntityType& type : geological_entity_types ) {
            print_nb_geological_entities( geomodel, type );
        }
    }

    void print_geomodel_mesh_stats( const GeoModel& geomodel )
    {
        Logger::out( "GeoModel", "Model ", geomodel.name(), " is made of\n",
            std::setw( 10 ), std::left, geomodel.mesh.vertices.nb(), " vertices\n",
            std::setw( 10 ), std::left, count_geomodel_edges( geomodel ), " edges" );

        index_t nb_triangles = geomodel.mesh.polygons.nb_triangle();
        index_t nb_quads = geomodel.mesh.polygons.nb_quad();
        index_t nb_unclassified_polygons =
            geomodel.mesh.polygons.nb_unclassified_polygon();
        index_t nb_polygons = geomodel.mesh.polygons.nb_polygons();
        Logger::out( "GeoModel", std::setw( 10 ), std::left, nb_polygons,
            " polygons" );
        if( nb_triangles > 0 ) {
            print_one_cell_stat( nb_triangles, nb_polygons, "triangles" );
        }
        if( nb_quads > 0 ) {
            print_one_cell_stat( nb_quads, nb_polygons, "quads" );
        }
        if( nb_unclassified_polygons > 0 ) {
            print_one_cell_stat( nb_unclassified_polygons, nb_polygons,
                "unclassified polygons" );
        }

        index_t nb_tet = geomodel.mesh.cells.nb_tet();
        index_t nb_pyramids = geomodel.mesh.cells.nb_pyramid();
        index_t nb_prisms = geomodel.mesh.cells.nb_prism();
        index_t nb_hex = geomodel.mesh.cells.nb_hex();
        index_t nb_poly = geomodel.mesh.cells.nb_connector();
        index_t nb_cells = geomodel.mesh.cells.nb_cells();
        Logger::out( "GeoModel", std::setw( 10 ), std::left, nb_cells, " cells" );
        if( nb_tet > 0 ) {
            print_one_cell_stat( nb_tet, nb_cells, "tet" );
        }
        if( nb_pyramids > 0 ) {
            print_one_cell_stat( nb_pyramids, nb_cells, "pyramids" );
        }
        if( nb_prisms > 0 ) {
            print_one_cell_stat( nb_prisms, nb_cells, "prisms" );
        }
        if( nb_hex > 0 ) {
            print_one_cell_stat( nb_hex, nb_cells, "hex" );
        }
        if( nb_poly > 0 ) {
            print_one_cell_stat( nb_poly, nb_cells, "polyhedra" );
        }
        Logger::out( "GeoModel" );
    }

    void print_geomodel_mesh_cell_volumes( const GeoModel& geomodel )
    {
        double tet_volume = 0;
        double pyramid_volume = 0;
        double prism_volume = 0;
        double hex_volume = 0;
        double poly_volume = 0;
        double volume = compute_geomodel_volumes_per_cell_type( geomodel, tet_volume,
            pyramid_volume, prism_volume, hex_volume, poly_volume );
        Logger::out( "GeoModel", "Model ", geomodel.name(), " has a volume of ",
            volume );
        if( tet_volume > 0 ) {
            print_one_cell_volume_stat( tet_volume, volume, "tet" );
        }
        if( pyramid_volume > 0 ) {
            print_one_cell_volume_stat( pyramid_volume, volume, "pyramid" );
        }
        if( prism_volume > 0 ) {
            print_one_cell_volume_stat( prism_volume, volume, "prism" );
        }
        if( hex_volume > 0 ) {
            print_one_cell_volume_stat( hex_volume, volume, "hex" );
        }
        if( poly_volume > 0 ) {
            print_one_cell_volume_stat( poly_volume, volume, "polyhedron" );
        }
        Logger::out( "GeoModel" );
    }

    bool are_geomodel_surface_meshes_simplicial( const GeoModel& geomodel )
    {
        for( index_t i = 0; i != geomodel.nb_surfaces(); ++i ) {
            if( !geomodel.surface( i ).is_simplicial() ) {
                return false;
            }
        }
        return true;
    }

    bool are_geomodel_region_meshes_simplicial( const GeoModel& geomodel )
    {
        for( index_t i = 0; i != geomodel.nb_regions(); ++i ) {
            if( !geomodel.region( i ).is_simplicial() ) {
                return false;
            }
        }
        return true;
    }

    index_t find_mesh_entity_id_from_name(
        const GeoModel& geomodel,
        const MeshEntityType& gmme_type,
        const std::string& name )
    {
        index_t mesh_entity_id = NO_ID;
        for( index_t elt_i = 0; elt_i < geomodel.nb_mesh_entities( gmme_type );
            elt_i++ ) {
            const RINGMesh::GeoModelMeshEntity& cur_gme = geomodel.mesh_entity(
                gmme_type, elt_i );
            if( cur_gme.name() == name ) {
                if( mesh_entity_id != NO_ID ) {
                    throw RINGMeshException( "GeoModel",
                        " At least two GeoModelMeshEntity have the same name in the GeoModel: "
                            + name );
                }
                mesh_entity_id = cur_gme.index();
            }
        }
        if( mesh_entity_id == NO_ID ) {
            throw RINGMeshException( "GeoModel",
                name
                    + " does not match with any actual GeoModelEntity name in the GeoModel" );
        }
        return mesh_entity_id;
    }

    index_t find_geological_entity_id_from_name(
        const RINGMesh::GeoModel& geomodel,
        const RINGMesh::GeologicalEntityType& gmge_type,
        const std::string& name )
    {
        index_t geological_entity_id = NO_ID;
        for( index_t elt_i = 0; elt_i < geomodel.nb_geological_entities( gmge_type );
            elt_i++ ) {
            const RINGMesh::GeoModelGeologicalEntity& cur_gme =
                geomodel.geological_entity( gmge_type, elt_i );
            if( cur_gme.name() == name ) {
                if( geological_entity_id != NO_ID ) {
                    throw RINGMeshException( "GeoModel",
                        "At least two GeoModelGeologicalEntity have the same name in the GeoModel : "
                            + name );
                }
                geological_entity_id = cur_gme.index();
            }
        }
        if( geological_entity_id == NO_ID ) {
            throw RINGMeshException( "GeoModel",
                name
                    + " does not match with any actual GeoModelEntity name in the GeoModel" );
        }
        return geological_entity_id;
    }

    /*******************************************************************************/

    void translate( GeoModel& M, const vec3& translation_vector )
    {
        for( index_t v = 0; v < M.mesh.vertices.nb(); ++v ) {
            // Coordinates are not directly modified to
            // update the matching vertices in geomodel entities
            const vec3& p = M.mesh.vertices.vertex( v );
            M.mesh.vertices.update_point( v, p + translation_vector );
        }
    }

    void rotate(
        GeoModel& M,
        const vec3& origin,
        const vec3& axis,
        double theta,
        bool degrees )
    {
        if( length( axis ) < M.epsilon() ) {
            Logger::err( "GeoModel",
                "Rotation around an epsilon length axis is impossible" );
            return;
        }
        GEO::Matrix< 4, double > rot_mat;
        rotation_matrix_about_arbitrary_axis( origin, axis, theta, degrees,
            rot_mat );

        for( index_t v = 0; v < M.mesh.vertices.nb(); ++v ) {
            const vec3& p = M.mesh.vertices.vertex( v );
            double old[4] = { p[0], p[1], p[2], 1. };
            double new_p[4] = { 0, 0, 0, 1. };
            GEO::mult( rot_mat, old, new_p );
            ringmesh_assert( std::fabs( new_p[3] - 1. ) < global_epsilon );

            M.mesh.vertices.update_point( v, vec3( new_p[0], new_p[1], new_p[2] ) );
        }
    }

#ifdef RINGMESH_WITH_TETGEN

    void tetrahedralize(
        GeoModel& M,
        const std::string& method,
        index_t region_id,
        bool add_steiner_points )
    {
        /* @todo Review: Maybe rethink these functions
         *       to have a function that can mesh a region of a geomodel
         *       taking only one vector of points [JP]
         */
        std::vector< std::vector< vec3 > > internal_vertices( M.nb_regions() );
        tetrahedralize( M, method, region_id, add_steiner_points,
            internal_vertices );
    }

    void tetrahedralize(
        GeoModel& M,
        const std::string& method,
        index_t region_id,
        bool add_steiner_points,
        const std::vector< std::vector< vec3 > >& internal_vertices )
    {
        if( region_id == NO_ID ) {
            Logger::out( "Info", "Using ", method );
            GEO::ProgressTask progress( "Compute", M.nb_regions() );
            for( index_t i = 0; i < M.nb_regions(); i++ ) {
                tetrahedralize( M, method, i, add_steiner_points,
                    internal_vertices );
                progress.next();
            }
        } else {
            std::unique_ptr< TetraGen > tetragen = TetraGen::create( M, region_id,
                method );
            tetragen->set_boundaries( M.region( region_id ), M.wells() );
            tetragen->set_internal_points( internal_vertices[region_id] );
            bool status = Logger::instance()->is_quiet();
            Logger::instance()->set_quiet( true );
            tetragen->tetrahedralize( add_steiner_points );
            Logger::instance()->set_quiet( status );
        }

        // The GeoModelMesh should be updated, just erase everything
        // and it will be re-computed during its next access.
        M.mesh.vertices.clear();
    }
#endif

}