/*---------------------------------------------------------------------------*\
                               |
  _____        _______ ____    | IWESOL: IWES Open Library
 |_ _\ \      / / ____/ ___|   |
  | | \ \ /\ / /|  _| \___ \   | Copyright: Fraunhofer Institute for Wind
  | |  \ V  V / | |___ ___) |  | Energy and Energy System Technology IWES
 |___|  \_/\_/  |_____|____/   |
                               | http://www.iwes.fraunhofer.de
                               |
-------------------------------------------------------------------------------
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of IWESOL and it is based on OpenFOAM.

    IWESOL and OpenFOAM are free software: you can redistribute them and/or modify
    them under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    IWESOL and OpenFOAM are distributed in the hope that they will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
	Foam::oldev::TerrainManager

Description
	A BlockMeshManager that creates a mesh above a terrain STL surface.

SourceFiles
	TerrainManager.C
	TMCalc.C

Author
    Jonas Schmidt (jonas.schmidt@iwes.fraunhofer.de)

Modified by


Test location
	$OLDEV_OF_TUT/utilities/mesh/terrainBlockMesher/basic

Tested by
	Jonas Schmidt

Used by


\*---------------------------------------------------------------------------*/

#ifndef TerrainManager_H
#define TerrainManager_H

#include "BlockMeshManager.H"
#include "STLLandscape.H"
#include "CoordinateSystem.H"
#include "modules/blockGrading/TerrainManagerModuleBlockGrading.H"
#include "modules/cylinder/TerrainManagerModuleCylinder.H"
#include "modules/orographyModifications/TerrainManagerModuleOrographyModifications.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

namespace oldev
{


/*---------------------------------------------------------------------------*\
                         Class TerrainManager Declaration
\*---------------------------------------------------------------------------*/

class TerrainManager
:
    public BlockMeshManager
{
    // Private data

    	/// The stl
    	const searchableSurface * const stl_;

    	/// the stl landscape
    	autoPtr< STLLandscape > landscape_;

    	/// the number of blocks
    	labelList blockNrs_;

    	/// the cell numbers
    	Vector< label > cellNrs_;

    	/// the domain box
    	Box domainBox_;

    	/// the stl box
    	Box stlBox_;

    	/// A point well above the stl
    	point p_above_;

    	/// The maximal distance searched during projection
    	scalar maxDistProj_;

    	/// Memory of point adressing
    	HashTable< label > pointAdr_;

    	/// Memory of point adressing
    	HashTable< label > pointAdr_ijv_;

    	/// Memory of block adressing
    	HashTable< label > blockAdr_ij_;

    	/// Memory of inverse block adressing
    	HashTable< labelList, label > blockAdrI_i_;


    	/// the distance for up splines
    	scalar splineNormalDist_;

    	/// the upward spline mode. 0 = nothing to do; 1 = all; 2 = ignoreBoundary
    	label mode_upwardSplines_;

    	/// the zero level for out-of-STL points
    	scalar zeroLevel_;

    	/// the grading factors
    	scalarList gradingFactors_;


    	/// the cylinder module
    	TerrainManagerModuleCylinder cylinderModule_;

    	/// the modify orography module
    	TerrainManagerModuleOrographyModifications modificationModule_;

    	/// the block grading module
    	TerrainManagerModuleBlockGrading gradingModule_;


    	/// the blending
    	autoPtr< ScalarBlendingFunction > blendingFunction_;

    	/// The spline point numbers
    	labelList splinePointNrs_;


    // Private Member Functions

    	/// Init the points, create blocks:
    	void initAll();

    	/// adds a point, returns its label
    	label getAddedPoint(const point & p, label upDown);

    	/// Returns the key for pointAdr
    	word hkey(const point & p, label upDown) const;

    	/// block contribution to patches
    	void contributeToPatches(label i, label j);

    	/// calculates orthogonal up splines
    	bool calcUpSplines();

    	/// BlockManager: calculate topology
    	bool calcTopology();

    	/// Move vertices onto landscape, returns success.
    	bool attachVertices(label blockI);

    	/// Calculate the vertex-vertex splines, returns success.
    	bool calcLandscapeSplines(label blockI);

        /// Disallow default bitwise copy construct
        TerrainManager(const TerrainManager&);

        /// Disallow default bitwise assignment
        void operator=(const TerrainManager&);


public:

    // declare friendship with modules:
    	friend class TerrainManagerModuleCylinder;
    	friend class TerrainManagerModuleOrographyModifications;
    	friend class TerrainManagerModuleBlockGrading;

    // Static data members

    	/// define directions
    	static const int UP, BASE1, BASE2;


    // Constructors

        /// Construct from components
        TerrainManager
        (
        		const Time & runTime,
    			const dictionary & dict,
    			const searchableSurface * const stl = 0
    	);


    // Member Functions

        // Access

        /// returns block numbers
        inline const labelList & blockNrs() const { return blockNrs_; }

        /// returns cell numbers
        inline const Vector< label > & cellNrs() const { return cellNrs_; }

    	/// returns ij labels from i
    	inline const labelList & i2ij(label i) const { return blockAdrI_i_[i]; }

    	/// returns i from labels ij
    	inline const label & ij2i(label i, label j) const { return blockAdr_ij_[key(i,j)]; }

    	/// returns coordinate system
    	const CoordinateSystem & coordinateSystem() const { return domainBox_.coordinateSystem(); }

    	/// walk around box labels, starting at SWL (or SWH), going east. returns (i,j,v).
    	labelList walkBox
    	(
    			label n,
    			const word & HL,
    			bool counterClockWise = true
    	) const;

    	/// returns box walk maximum number
    	inline label walkBoxMaximum() const;


        // Edit

    	/// Starts the calculation
    	bool calc();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace oldev

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "TerrainManagerI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
