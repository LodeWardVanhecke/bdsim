#include "G4BooleanProcessorCGAL.hh"
#include "G4Polyhedron.hh"
#include "G4PolyhedronArbitrary.hh"
#include "G4SurfaceMeshCGAL.hh"

G4PolyhedronArbitrary* G4BooleanProcessorCGAL::Intersection(G4Polyhedron* p1, G4Polyhedron* p2) {
    G4SurfaceMeshCGAL *sm1 = new G4SurfaceMeshCGAL();
    G4SurfaceMeshCGAL *sm2 = new G4SurfaceMeshCGAL();
    sm1->fill(p1);
    sm2->fill(p2);
    G4SurfaceMeshCGAL *sm3 = sm1->Intersection(sm2);
    G4PolyhedronArbitrary *ap = sm3->GetPolyhedronArbitrary();
    delete sm1;
    delete sm2;
    delete sm3;
    return ap;
}

G4PolyhedronArbitrary* G4BooleanProcessorCGAL::Union(G4Polyhedron* p1, G4Polyhedron* p2) {
    G4SurfaceMeshCGAL *sm1 = new G4SurfaceMeshCGAL();
    G4SurfaceMeshCGAL *sm2 = new G4SurfaceMeshCGAL();
    sm1->fill(p1);
    sm2->fill(p2);
    G4SurfaceMeshCGAL *sm3 = sm1->Union(sm2);
    G4PolyhedronArbitrary *ap = sm3->GetPolyhedronArbitrary();
    delete sm1;
    delete sm2;
    delete sm3;
    return ap;
}

G4PolyhedronArbitrary* G4BooleanProcessorCGAL::Subtraction(G4Polyhedron* p1, G4Polyhedron* p2) {
    G4SurfaceMeshCGAL *sm1 = new G4SurfaceMeshCGAL();
    G4SurfaceMeshCGAL *sm2 = new G4SurfaceMeshCGAL();
    sm1->fill(p1);
    sm2->fill(p2);
    G4SurfaceMeshCGAL *sm3 = sm1->Subtraction(sm2);
    G4PolyhedronArbitrary *ap = sm3->GetPolyhedronArbitrary();
    delete sm1;
    delete sm2;
    delete sm3;
    return ap;
}