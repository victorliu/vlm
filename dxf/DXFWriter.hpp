#ifndef DXFWRITER_HPP_INCLUDED
#define DXFWRITER_HPP_INCLUDED

#include <iostream>
#include <vector>

template <typename T>
class DXFWriter{
protected:
	std::ostream *ptr_os;
	T width;
	int layer;
public:
	typedef T real_type;
	
	struct HeaderOptions{
		enum{ // $INSUNITS
			DXF_UNITS_NONE              =  0,
			DXF_UNITS_INCHES            =  1,
			DXF_UNITS_FEET              =  2,
			DXF_UNITS_MILES             =  3,
			DXF_UNITS_MILLIMETERS       =  4,
			DXF_UNITS_CENTIMETERS       =  5,
			DXF_UNITS_METERS            =  6,
			DXF_UNITS_KILOMETERS        =  7,
			DXF_UNITS_MICROINCHES       =  8,
			DXF_UNITS_MILS              =  9,
			DXF_UNITS_YARDS             = 10,
			DXF_UNITS_ANGSTROMS         = 11,
			DXF_UNITS_NANOMETERS        = 12,
			DXF_UNITS_MICRONS           = 13,
			DXF_UNITS_DECIMETERS        = 14,
			DXF_UNITS_DECAMETERS        = 15,
			DXF_UNITS_HECTOMETERS       = 16,
			DXF_UNITS_GIGAMETERS        = 17,
			DXF_UNITS_ASTRONOMICALUNITS = 18,
			DXF_UNITS_LIGHTYEARS        = 19,
			DXF_UNITS_PARSECS           = 20
		} units;
	};
	struct TableOptions{
		std::vector<int> layers;
	};
public:
	DXFWriter():ptr_os(&std::cout),width(T(0)),layer(0){}
	virtual ~DXFWriter(){}
	
	std::ostream* SetStream(std::ostream *osnew){
		std::ostream* old = ptr_os;
		ptr_os = osnew;
		return old;
	}
	T SetWidth(const T &w){
		T ret = width;
		width = w;
		return ret;
	}
	int SetLayer(int l){
		int ret = layer;
		layer = l;
		return ret;
	}
	int WriteHeader(const HeaderOptions *opts = NULL) const{
		std::ostream &os = *ptr_os;
		// === Header section of every dxf file. 
		os << 0 << '\n';
		os << "SECTION" << '\n';
		os << 2 << '\n';
		os << "HEADER" << '\n';
		os << 9 << '\n';
		os << "$ACADVER" << '\n';
		os << 1 << '\n';
		os << "AC1006" << '\n';
		/*
		// Set Base command (in WCS)
		os << 9 << '\n';
		os << "$INSBASE" << '\n';
		os << 10 << '\n';
		os << 0.0 << '\n';
		os << 20 << '\n';
		os << 0.0 << '\n';
		os << 30 << '\n';
		os << 0.0 << '\n';

		if(NULL != opts){
			os << 9 << '\n';
			os << "$INSUNITS" << '\n';
			os << 70 << '\n';
			os << (int)opts->units << '\n';
		}
		*/
		/*
		// Drawing lower left corner
		os << 9 << '\n';
		os << "$EXTMIN" << '\n';
		os << 10 << '\n';
		os << 0.0 << '\n';
		os << 20 << '\n';
		os << 0.0 << '\n';

		// Drawing upper right corner
		os << 9 << '\n';
		os << "$EXTMAX" << '\n';
		os << 10 << '\n';
		os << 10000.0 << '\n';
		os << 20 << '\n';
		os << 10000.0 << '\n';
		*/

		os << 0 << '\n';
		os << "ENDSEC" << '\n'; 
		// === End of header section

		return 0;
	}
	
	int WriteFooter() const{
		std::ostream &os = *ptr_os;
		// end of sequence objects of dxf file.
		os << 0 << '\n';
		os << "ENDSEC" << '\n';
		os << 0 << '\n';
		os << "EOF" << '\n';
		return 0;
	}
	
	int WriteTables(const TableOptions *opts = NULL) const{
		if(NULL == opts){ return 0; }
		std::ostream &os = *ptr_os;
		os << 0 << '\n';
		os << "SECTION" << '\n';
		os << 2 << '\n';
		os << "TABLES" << '\n';
		if(opts->layers.size() > 0){
			os << 0 << '\n';
			os << "TABLE" << '\n';
			os << 2 << '\n';
			os << "LAYER" << '\n';
			/*
			os << 100 << '\n';
			os << "AcDbSymbolTable" << '\n';
			*/
			os << 70 << '\n';
			os << opts->layers.size() << '\n';
			for(size_t i = 0; i < opts->layers.size(); ++i){
				int layer = opts->layers[i];
				os << 0 << '\n';
				os << "LAYER" << '\n';
				/*
				os << 100 << '\n';
				os << "AcDbSymbolTableRecord" << '\n';
				*/
				os << 70 << '\n'; // standard flags
				os << 0 << '\n'; // 64 = layer was referenced at least once
				os << 62 << '\n'; // color number
				os << (i+1) << '\n'; // white
				os << 6 << '\n'; // linetype name
				os << "CONTINUOUS" << '\n';
				os << 2 << '\n'; // layer name
				os << layer << '\n';
			}
		}
		os << 0 << '\n';
		os << "ENDTAB" << '\n';
		os << 0 << '\n';
		os << "ENDSEC" << '\n';
		return 0;
	}
	
	int WriteBlocks() const{
		std::ostream &os = *ptr_os;
		// Entities section
		os << 0 << '\n';
		os << "SECTION" << '\n';
		os << 2 << '\n';
		os << "BLOCKS" << '\n';
		os << 0 << '\n';
		os << "ENDSEC" << '\n';
		return 0;
	}
	int WriteEntities() const{
		std::ostream &os = *ptr_os;
		// Entities section
		os << 0 << '\n';
		os << "SECTION" << '\n';
		os << 2 << '\n';
		os << "ENTITIES" << '\n';
		return 0;
	}

	int WriteCircle(const T &cx, const T &cy, const T &r) const{
		std::ostream &os = *ptr_os;
		// Draw the circle
		os << 0    << '\n';
		os << "CIRCLE" << '\n';

		os << 8 << '\n';
		os << layer    << '\n';    // Layer number (default layer in autocad)
		/*
		os << 39 << '\n';
		os << width    << '\n';    // Line thickness
		*/
		os << 10 << '\n';    // XYZ is the Center point of circle
		os << cx << '\n';    // X in UCS (User Coordinate System)coordinates

		os << 20 << '\n';
		os << cy << '\n';    // Y in UCS (User Coordinate System)coordinates
		/*
		os << 30 << '\n';
		os << 0.0 << '\n';    // Z in UCS (User Coordinate System)coordinates
		*/
		os << 40 << '\n';
		os << r << '\n';    // radius of circle
		return 0;
	}
	int WriteLine(const T &x1, const T &y1, const T &x2, const T &y2) const{
		std::ostream &os = *ptr_os;
		// Draw the circle
		os << 0          << '\n';
		os << "LINE"  << '\n';
		os << 8          << '\n';
		os << layer          << '\n';    // Layer number (default layer in autocad)
		/*
		os << 39 << '\n';
		os << width    << '\n';    // Line thickness
		*/
		/*
		T z1 = 0.0f;
		T z2 = 0.0f;
		*/
		os << 10    << '\n';    // XYZ is the Center point of circle
		os << x1    << '\n';    // X in UCS (User Coordinate System)coordinates
		os << 20    << '\n';
		os << y1    << '\n';    // Y in UCS (User Coordinate System)coordinates
		/*
		os << 30    << '\n';
		os << z1    << '\n';    // Z in UCS (User Coordinate System)coordinates
		*/
		os << 11    << '\n';    // XYZ is the Center point of circle
		os << x2    << '\n';    // X in UCS (User Coordinate System)coordinates
		os << 21    << '\n';
		os << y2    << '\n';    // Y in UCS (User Coordinate System)coordinates
		/*
		os << 31    << '\n';
		os << z2    << '\n';    // Z in UCS (User Coordinate System)coordinates
		*/
		return 0;
	}
	int WriteBox(const T &x1, const T &y1, const T &x2, const T &y2) const{
		std::vector<T> xy(8);
		xy[0] = x1; xy[1] = y1;
		xy[2] = x2; xy[3] = y1;
		xy[4] = x2; xy[5] = y2;
		xy[6] = x1; xy[7] = y2;
		return WritePolyline(xy);
	}
	int WritePolyline(const std::vector<T> &xy) const{
		std::ostream &os = *ptr_os;
		const size_t count = xy.size()/2;
		if(0 == count){ return -1; }

		os << 0 << '\n';
		os << "POLYLINE"  << '\n';

		os << 8 << '\n';
		os << layer << '\n';    // Layer number (default layer in autocad)

		os << 70 << '\n'; // Polyline flags
		os << 1 << '\n'; // closed polyline
		os << 40 << '\n'; // Start width
		os << 0 << '\n'; // default
		os << 41 << '\n'; // End width
		os << 0 << '\n'; // default
		os << 66 << '\n'; // Obsolete; formerly an “entities follow flag” (optional; ignore if present)
		os << 1 << '\n';
		/*
		os << 39 << '\n';
		os << width    << '\n';    // Line thickness
		*/
		/*
		os << 100 << '\n';    // Subclass marker (AcDb2dPolyline or AcDb3dPolyline)
		os << "AcDb2dPolyline" << '\n';
		*/
		/*
		// DXF X,Y always 0
		os << 10 << '\n';
		os << 0.0f << '\n';

		os << 20 << '\n';
		os << 0.0f << '\n';

		// DXF Polyline elevation
		os << 30 << '\n';
		os << 0.0f << '\n';
		*/
		// Per-vertex info
		for(size_t i = 0; i < count; ++i){
			os << 0 << '\n';
			os << "VERTEX"  << '\n';

			/*
			5 // Handle
			FD

			330 // Soft-pointer ID/handle to owner dictionary (optional)
			FC

			100 // Subclass marker (AcDbEntity)
			AcDbEntity
			*/

			os << 8 << '\n';
			os << layer << '\n';    // Layer number (default layer in autocad)

			/*
			os << 62 << '\n';    // Color number
			os << 196 << '\n';

			os << 420 << '\n';    // 24 bit color value
			os << 3737736 << '\n';
			*/
			/*
			os << 100 << '\n';    // Subclass marker (AcDb2dVertex)
			os << "AcDb2dVertex" << '\n';
			*/
			os << 10 << '\n';
			os << xy[2*i+0] << '\n';

			os << 20 << '\n';
			os << xy[2*i+1] << '\n';
			/*
			os << 30 << '\n';
			os << 0 << '\n';
			*/
		}

		os << 0 << '\n'; // Sequence end marker
		os << "SEQEND" << '\n';
		return 0;
	}
	int WriteArc(const T &cx, const T &cy, const T &r, const T &start_angle_deg = 0, const T &end_angle_deg = 360) const{
		std::ostream &os = *ptr_os;
		// Draw an arc
		os << 0    << '\n';
		os << "ARC" << '\n';

		// General entity params
		/*
		os << 100 << '\n';
		os << "AcDbEntity" << '\n';
		*/
		os << 8 << '\n';
		os << layer    << '\n';    // Layer number (default layer in autocad)

		// Circle subclass
		/*
		os << 100 << '\n';
		os << "AcDbCircle" << '\n';
		*/
		os << 39 << '\n';
		os << width    << '\n';    // Line thickness

		os << 10 << '\n';    // XYZ is the Center point of an arc
		os << cx << '\n';    // X in UCS (User Coordinate System)coordinates

		os << 20 << '\n';
		os << cy << '\n';    // Y in UCS (User Coordinate System)coordinates
		/*
		os << 30 << '\n';
		os << 0.0 << '\n';    // Z in UCS (User Coordinate System)coordinates
		*/
		os << 40 << '\n';
		os << r << '\n';    // arc radius

		// Arc subclass params
		/*
		os << 100 << '\n';
		os << "AcDbArc" << '\n';
		*/
		// Start and end angles
		os << 50 << '\n';
		os << start_angle_deg << '\n';

		os << 51 << '\n';
		os << end_angle_deg << '\n';
		return 0;
	}
	int WritePoint(const T &x, const T &y) const{
		std::ostream &os = *ptr_os;
		os << 0 << '\n'; // entity type
		os << "POINT" << '\n';

		os << 8 << '\n'; // layer name
		os << layer << '\n';
			
		os << 10 << '\n';
		os << x << '\n';
			
		os << 20 << '\n';
		os << y << '\n';
		/*
		os << 30 << '\n';
		os << 0.0 << '\n';
		*/
		return 0;
	}
};

#endif // DXFWRITER_HPP_INCLUDED
