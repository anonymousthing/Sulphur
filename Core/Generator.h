#pragma once

#include <vector>
#include <tuple>

class Generator {
public:
	virtual ~Generator();

	//Parameter access
	bool setParameter(const char *parameterName, void *value) {
		return setParameter(getParameterHandle(parameterName), value);
	}

	bool getParameter(const char *parameterName, void **value) const {
		return getParameter(getParameterHandle(parameterName), value);
	}
	
	bool setParameter(int parameterHandle, void *value) {
		if (parameterHandle < 0 || parameterHandle >= parameters.size()) {
			return false;
		}

		auto &pair = parameters[parameterHandle];
		if (!std::get<2>(pair)(value)) {
			return false;
		}

		*std::get<1>(pair) = value;
	}

	bool getParameter(int parameterHandle, void **value) const  {
		if (parameterHandle < 0 || parameterHandle >= parameters.size()) {
			return false;
		}

		auto &pair = parameters[parameterHandle];
		*value = *std::get<1>(pair);
		return true;
	}

	int getParameterHandle(const char *parameterName) const  {
		int index = 0;
		for (auto pi = parameters.begin(), pe = parameters.end(); pi != pe; ++pi, index++) {
			const auto &tuple = *pi;
			if (strcmp(parameterName, std::get<0>(tuple)) == 0) {
				return index;
			}
		}
		return -1;
	}

protected:
	//Protected to ensure that we must be overridden.
	Generator() { }

	typedef bool (*ParameterValidator)(void*);

	int registerParameter(const char *parameterName, void **parameter, ParameterValidator validator) {
		parameters.push_back(std::make_tuple(parameterName, parameter, validator));

		return 12;
	}

private:
	std::vector<std::tuple<const char *, void **, ParameterValidator>> parameters;
};
