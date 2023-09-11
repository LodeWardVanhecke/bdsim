#include "G4BooleanProcessorCGAL.hh"
#include "G4Polyhedron.hh"
#include "G4PolyhedronArbitrary.hh"
#include "G4SurfaceMeshCGAL.hh"
#include "G4PolyhedronArbitrary.hh"

G4PolyhedronArbitrary* G4BooleanProcessorCGAL::Intersection(G4Polyhedron* p1, G4Polyhedron* p2) {
    G4cout << "Intersection " << p1 << " " << p2 << G4endl;

    G4SurfaceMeshCGAL *sm1 = new G4SurfaceMeshCGAL();
    G4SurfaceMeshCGAL *sm2 = new G4SurfaceMeshCGAL();

    if (p1 != nullptr)
        sm1->fill(p1);
    if (p2 != nullptr)
        sm2->fill(p2);

    if (p1 == nullptr && p2 == nullptr)
        return new G4PolyhedronArbitrary(0,0);

    G4cout << sm1->IsValid() << " " << sm1->IsTriangular() << " " << sm1->IsOutwardOriented() << " " << sm1->IsClosed() << " " << sm1->IsValidHalfEdgeGraph() << " " << sm1->BoundAVolume() << " "
           << sm1->NumberOfVertices() << " " << sm1->NumberOfFaces() << " " << sm1->Volume() << " "
           << sm2->IsValid() << " " << sm1->IsTriangular() << " " << sm2->IsOutwardOriented() << " " << sm2->IsClosed() << " " << sm1->IsValidHalfEdgeGraph() << " " << sm2->BoundAVolume() << " "
           << sm2->NumberOfVertices() << " " << sm2->NumberOfFaces() << " " << sm2->Volume() <<G4endl;

    sm1->WriteMesh("mesh1.off");
    sm2->WriteMesh("mesh2.off");

    G4cout << "Intersection executing" <<  G4endl;
    G4SurfaceMeshCGAL *sm3 = sm1->Intersection(sm2);
    G4cout << "Intersection done" <<  G4endl;
    G4PolyhedronArbitrary *ap = sm3->GetPolyhedronArbitrary();
    delete sm1;
    delete sm2;
    delete sm3;
    return ap;
}

G4PolyhedronArbitrary* G4BooleanProcessorCGAL::Union(G4Polyhedron* p1, G4Polyhedron* p2) {
    G4cout << "Union " << p1 << " " << p2 << G4endl;

    G4SurfaceMeshCGAL *sm1 = new G4SurfaceMeshCGAL();
    G4SurfaceMeshCGAL *sm2 = new G4SurfaceMeshCGAL();

    if (p1 != nullptr)
        sm1->fill(p1);
    if (p2 != nullptr)
        sm2->fill(p2);

    if (p1 == nullptr && p2 == nullptr)
        return new G4PolyhedronArbitrary(0,0);

    G4cout << sm1->IsValid() << " " << sm1->IsTriangular() << " " << sm1->IsOutwardOriented() << " " << sm1->IsClosed() << " " << sm1->IsValidHalfEdgeGraph() << " " << sm1->BoundAVolume() << " "
           << sm1->NumberOfVertices() << " " << sm1->NumberOfFaces() << " " << sm1->Volume() << " "
           << sm2->IsValid() << " " << sm1->IsTriangular() << " " << sm2->IsOutwardOriented() << " " << sm2->IsClosed() << " " << sm1->IsValidHalfEdgeGraph() << " " << sm2->BoundAVolume() << " "
           << sm2->NumberOfVertices() << " " << sm2->NumberOfFaces() << " " << sm2->Volume() <<G4endl;

    //sm1->DecomposeConnected();
    //sm2->DecomposeConnected();

    sm1->WriteMesh("mesh1.off");
    sm2->WriteMesh("mesh2.off");


    G4cout << "Union executing" <<  G4endl;
    G4SurfaceMeshCGAL *sm3 = sm1->Union(sm2);
    G4cout << "Union done" <<  G4endl;
    G4PolyhedronArbitrary *ap = sm3->GetPolyhedronArbitrary();
    delete sm1;
    delete sm2;
    delete sm3;
    return ap;
}

G4PolyhedronArbitrary* G4BooleanProcessorCGAL::Subtraction(G4Polyhedron* p1, G4Polyhedron* p2) {
    G4cout << "Subtraction " << p1 << " " << p2 << G4endl;

    G4SurfaceMeshCGAL *sm1 = new G4SurfaceMeshCGAL();
    G4SurfaceMeshCGAL *sm2 = new G4SurfaceMeshCGAL();

    if (p1 != nullptr)
        sm1->fill(p1);
    if (p2 != nullptr)
        sm2->fill(p2);

    if (p1 == nullptr && p2 == nullptr)
        return new G4PolyhedronArbitrary(0,0);

    G4cout << sm1->IsValid() << " " << sm1->IsTriangular() << " " << sm1->IsOutwardOriented() << " " << sm1->IsClosed() << " " << sm1->IsValidHalfEdgeGraph() << " " << sm1->BoundAVolume() << " "
           << sm1->NumberOfVertices() << " " << sm1->NumberOfFaces() << " " << sm1->Volume() << " "
           << sm2->IsValid() << " " << sm1->IsTriangular() << " " << sm2->IsOutwardOriented() << " " << sm2->IsClosed() << " " << sm1->IsValidHalfEdgeGraph() << " " << sm2->BoundAVolume() << " "
           << sm2->NumberOfVertices() << " " << sm2->NumberOfFaces() << " " << sm2->Volume() <<G4endl;

    sm1->WriteMesh("mesh1.off");
    sm2->WriteMesh("mesh2.off");

    G4cout << "Subtraction executing"  << G4endl;
    G4SurfaceMeshCGAL *sm3 = sm1->Subtraction(sm2);
    G4cout << "Subtraction done" <<  G4endl;
    G4PolyhedronArbitrary *ap = sm3->GetPolyhedronArbitrary();

    delete sm1;
    delete sm2;
    delete sm3;
    return ap;
}