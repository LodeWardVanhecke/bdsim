simple_fail(rfcavity-invalid-shape "--file=rfcavity-invalid-shape.gmad")
simple_fail(rfcavity-invalid-radius "--file=rfcavity-invalid-radius.gmad")
simple_testing(rfcavity-geometry-elliptical  "--file=rfcavity-geometry-elliptical.gmad"  ${OVERLAP_CHECK})
simple_testing(rfcavity-geometry-rectangular "--file=rfcavity-geometry-rectangular.gmad" ${OVERLAP_CHECK})
simple_testing(rfcavity-geometry-pillbox     "--file=rfcavity-geometry-pillbox.gmad"     ${OVERLAP_CHECK})
