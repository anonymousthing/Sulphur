#pragma once

#include "Model.h"
#include "Matrix.h"

class ModelInstance {
public:
	Model *model;
	Matrix4x4 *transform;

	ModelInstance(Model *model, const Matrix4x4 &outerTransform) : model(model) {
		transform = new Matrix4x4(outerTransform);
	}

	~ModelInstance() {
		delete transform;
	}
};
