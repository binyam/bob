#ifndef PLANE_INC
#define PLANE_INC

#include "ip/Point3D.h"
#include "ip/Vector3D.h"

namespace Torch {

	/** This class is designed to handle a plane

	    A Plane is defined by a point P0 and a normal n.

	    @author Sebastien Marcel (marcel@idiap.ch)
	    @author Dan Sunday (http://www.softsurfer.com/)
	    @version 2.0
	    \date
	    @since 1.0
	*/
	class Plane : public geomObject
	{
	public:
		/// 3D point
		Point3D P0;

		/// 3D normal vector
		Vector3D n;

		//-----

		/** @name constructors */
		//@{
		/// empty constructor
		Plane() {};
		/// creates a plane from a point and a normal
		Plane(const Point3D& P0_, const Vector3D& n_) { P0 = P0_; n = n_; }
		//@}

		/// destructor
		virtual ~Plane() {};

		//-----

		/// draw the plane on a image
		virtual void draw(Image *image_, Color color_);
	};

}

#endif