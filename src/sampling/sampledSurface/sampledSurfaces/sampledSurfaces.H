/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::sampledSurfaces

Description
    Set of surfaces to sample.

    The write() method is used to sample and write files.

SourceFiles
    sampledSurfaces.C

\*---------------------------------------------------------------------------*/

#ifndef sampledSurfaces_H
#define sampledSurfaces_H

#include "sampledSurface.H"
#include "surfaceWriter.H"
#include "volFieldsFwd.H"
#include "surfaceFieldsFwd.H"
#include "wordReList.H"
#include "IOobjectList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class fvMesh;
class dictionary;

/*---------------------------------------------------------------------------*\
                       Class sampledSurfaces Declaration
\*---------------------------------------------------------------------------*/

class sampledSurfaces
:
    public PtrList<sampledSurface>
{
    // Private classes

        //- Class used for surface merging information
        class mergeInfo
        {
        public:
            pointField points;
            faceList   faces;
            labelList  pointsMap;

            //- Clear all storage
            void clear()
            {
                points.clear();
                faces.clear();
                pointsMap.clear();
            }
        };


    // Static data members

        //- output verbosity
        static bool verbose_;

        //- Tolerance for merging points (fraction of mesh bounding box)
        static scalar mergeTol_;

    // Private data

        //- Name of this set of surfaces,
        //  Also used as the name of the sampledSurfaces directory.
        const word name_;

        //- Const reference to fvMesh
        const fvMesh& mesh_;

        //- Load fields from files (not from objectRegistry)
        const bool loadFromFiles_;

        //- output path
        fileName outputPath_;


        // Read from dictonary

            //- Names of fields to sample
            wordReList fieldSelection_;

            //- Interpolation scheme to use
            word interpolationScheme_;

        // surfaces

            //- Information for merging surfaces
            List<mergeInfo> mergeList_;


        // Calculated

            //- Surface formatter
            autoPtr<surfaceWriter> formatter_;


    // Private Member Functions


        // Return number of fields
        label classifyFields();

        //- Write geometry only
        void writeGeometry() const;

        //- Write sampled fieldName on surface and on outputDir path
        template<class Type>
        void writeSurface
        (
            const Field<Type>& values,
            const label surfI,
            const word& fieldName,
            const fileName& outputDir
        );

        //- Sample and write a particular volume field
        template<class Type>
        void sampleAndWrite
        (
            const GeometricField<Type, fvPatchField, volMesh>&
        );

        //- Sample and write a particular surface field
        template<class Type>
        void sampleAndWrite
        (
            const GeometricField<Type, fvsPatchField, surfaceMesh>&
        );

        //- Sample and write all sampled fields
        template<class Type>
        void sampleAndWrite(const fvMesh&);

        //- Disallow default bitwise copy construct and assignment
        sampledSurfaces(const sampledSurfaces&);
        void operator=(const sampledSurfaces&);


public:

    //- Runtime type information
    TypeName("surfaces");


    // Constructors

        //- Construct for given objectRegistry and dictionary
        //  allow the possibility to load fields from files
        sampledSurfaces
        (
            const word& name,
            const objectRegistry&,
            const dictionary&,
            const bool loadFromFiles = false
        );


    //- Destructor
    virtual ~sampledSurfaces();


    // Member Functions

        //- Does any of the surfaces need an update?
        virtual bool needsUpdate() const;

        //- Mark the surfaces as needing an update.
        //  May also free up unneeded data.
        //  Return false if all surfaces were already marked as expired.
        virtual bool expire();

        //- Update the surfaces as required and merge surface points (parallel).
        //  Return false if no surfaces required an update.
        virtual bool update();


        //- Return name of the set of surfaces
        virtual const word& name() const
        {
            return name_;
        }

        //- set verbosity level
        void verbose(const bool verbosity = true);

        //- Execute, currently does nothing
        virtual void execute();

        //- Execute at the final time-loop, currently does nothing
        virtual void end();

        //- Sample and write
        virtual void write();

        //- Read the sampledSurfaces dictionary
        virtual void read(const dictionary&);

        //- Update for changes of mesh - expires the surfaces
        virtual void updateMesh(const mapPolyMesh&);

        //- Update for mesh point-motion - expires the surfaces
        virtual void movePoints(const pointField&);

        //- Update for changes of mesh due to readUpdate - expires the surfaces
        virtual void readUpdate(const polyMesh::readUpdateState state);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "sampledSurfacesTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
