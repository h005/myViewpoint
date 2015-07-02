//=============================================================================
//
//                               OpenMesh
//      Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
//                           www.openmesh.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//   This library is free software; you can redistribute it and/or modify it
//   under the terms of the GNU Library General Public License as published
//   by the Free Software Foundation, version 2.
//
//   This library is distributed in the hope that it will be useful, but
//   WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Library General Public License for more details.
//
//   You should have received a copy of the GNU Library General Public
//   License along with this library; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 83 $
//   $Date: 2009-02-27 17:31:45 +0100 (Fri, 27 Feb 2009) $
//
//=============================================================================


#include <iostream>
// -------------------- OpenMesh
#include "common.hh"
#include "gausscurvature.hh"
#include "meancurvature.hh"

int main()
{
    MyMesh mesh;
    // read mesh from stdin
    if ( ! OpenMesh::IO::read_mesh(mesh, "1.off") )
    {
        std::cerr << "Error: Cannot read mesh from " << std::endl;
        return 1;
    }

    GaussCurvature<MyMesh> a(mesh);
    MeanCurvature<MyMesh> b(mesh);
    std::cout << "finished" << std::endl;
    return 0;
}
