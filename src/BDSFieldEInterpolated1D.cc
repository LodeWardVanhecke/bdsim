/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSDimensionType.hh"
#include "BDSFieldEInterpolated1D.hh"
#include "BDSInterpolator1D.hh"

#include "G4ThreeVector.hh"

BDSFieldEInterpolated1D::BDSFieldEInterpolated1D(BDSInterpolator1D* interpolatorIn,
						 G4Transform3D      offset,
						 G4double           eScalingIn):
  BDSFieldEInterpolated(offset, eScalingIn),
  interpolator(interpolatorIn),
  dimensionIndex((interpolatorIn->Dimension()).underlying()),
  time((interpolatorIn->Dimension()).underlying() > 2)
{;}

BDSFieldEInterpolated1D::~BDSFieldEInterpolated1D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldEInterpolated1D::GetField(const G4ThreeVector& position,
						const G4double       t) const
{
  G4double coordinate = 0;
  if (time)
    {coordinate = t;}
  else
    {coordinate = position[dimensionIndex];}
  return interpolator->GetInterpolatedValue(coordinate) * EScaling();
}
