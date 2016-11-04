#include "DebugGraphics.h"

DebugGraphics::DebugGraphics() {

}

void DebugGraphics::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
	float segment[6] = { from.getX(), from.getY(), from.getZ(), to.getX(), to.getY(), to.getZ() };
	float colours[6] = { color.getX(), color.getY(), color.getZ(), color.getX(), color.getY(), color.getZ() };

	//Push the vertices of the line on to the buffer
	linesBuffer.push_back(from.getX());
	linesBuffer.push_back(from.getY());
	linesBuffer.push_back(from.getZ());
	linesBuffer.push_back(to.getX());
	linesBuffer.push_back(to.getY());
	linesBuffer.push_back(to.getZ());

	//We want the same colour for both vertices, so push the colour onto the buffer twice (one for each vertex)
	colorBuffer.push_back(color.getX());
	colorBuffer.push_back(color.getY());
	colorBuffer.push_back(color.getZ());

	colorBuffer.push_back(color.getX());
	colorBuffer.push_back(color.getY());
	colorBuffer.push_back(color.getZ());
}
