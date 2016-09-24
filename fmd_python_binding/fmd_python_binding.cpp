#include "fmd_python_binding.h"

#include "FuzzyMultiDataExtAlgorithm.h"

#include "boost/python.hpp"
#include "boost/python/stl_iterator.hpp"
#include "boost/python/exception_translator.hpp"
#include "boost/numpy.hpp"

#include <string>

namespace py = boost::python;
namespace np = boost::numpy;

AxiomLib::MultiTS fill_ts(const np::ndarray& array)
{
    AxiomLib::MultiTS result;

    if((array.get_flags() & np::ndarray::C_CONTIGUOUS)
            && array.get_dtype() == np::dtype::get_builtin<double>()) {
        int rows = array.shape(0);
        int cols = array.shape(1);
        double* data = reinterpret_cast<double*>(array.get_data());
        result.validParams.assign(rows, true);
        result.data.resize(rows);
        for(size_t col = 0; col < cols; ++col) {
            result.data[col].assign(data, data + cols);
            data += cols;
        }
    } else if (!(array.get_flags() & np::ndarray::C_CONTIGUOUS)){
        throw std::runtime_error("fill_ts(): array representing time series is not C_CONTIGUOUS");
    } else {
        throw std::runtime_error("fill_ts(): dtype of array representing time series is not double (float64)");
    }

    return result;
}

AxiomLib::ClassTS fill_class(const py::object& ts_list)
{
    AxiomLib::ClassTS result;
    result.reserve(py::len(ts_list));
    py::stl_input_iterator<np::ndarray> ts_iter(ts_list), end;
    for(; ts_iter != end; ts_iter++) {
        result.push_back(fill_ts(*ts_iter));
    }

    return result;
}

AxiomLib::DataSetDivision fill_division(const py::object& class_list) {
    py::stl_input_iterator<py::object> class_iter(class_list), end;
    AxiomLib::ReferenceClassesTS abnormal;
    AxiomLib::ClassTS normal;
    for(; class_iter != end; ++class_iter) {
        if (normal.empty()) {
            normal = fill_class(*class_iter);
        } else {
            abnormal.push_back(fill_class(*class_iter));
        }
    }

    return AxiomLib::DataSetDivision(abnormal, normal);
}

AxiomLib::RecognizerExt* train_FMD(py::dict env_dict, py::list dataSet_list)
{
    try {
        // Construct Environment
        AxiomLib::Environment env;
        {
            py::stl_input_iterator<py::tuple> iter(env_dict.iteritems()), end;
            for (; iter != end; ++iter) {
                py::tuple pair = *iter;
                if (py::len(pair) != 2) {
                    throw std::runtime_error("train_FMD(): Failed to iterate over env_dict items");
                }

                py::extract<std::string> key(pair[0]);
                py::extract<std::string> value(pair[1]);
                py::extract<py::list> value_list(pair[1]);
                if (!key.check()) {
                    throw std::runtime_error("train_FMD(): could not convert env_dict key to string");
                }
                if (value_list.check()) {
                    py::stl_input_iterator<std::string> list_iter(value_list()), end;
                    for(; list_iter != end; ++list_iter) {
                        env.setParamValue(key(), *list_iter);
                    }
                } else if (value.check()) {
                    env.setParamValue(key(), value());
                } else {
                    throw std::runtime_error("train_FMD(): could not convert env_dict value to string or list");
                }
            }
        }

        // Construct FuzzyDataSet

        AxiomLib::FuzzyDataSet* dataSet = new AxiomLib::FuzzyDataSet();

        std::vector<AxiomLib::DataSetDivision> divisions;
        divisions.reserve(AxiomLib::FuzzyDataSet::Last - AxiomLib::FuzzyDataSet::First);

        py::stl_input_iterator<py::object> dataset_divs_iter(dataSet_list), end;
        size_t div_index = 0;
        for(; dataset_divs_iter != end; ++dataset_divs_iter, ++div_index) {
            divisions.push_back(fill_division(*dataset_divs_iter));
        }

        divisions.resize(AxiomLib::FuzzyDataSet::Last - AxiomLib::FuzzyDataSet::First);

        dataSet->setDivisions(divisions);

        AxiomLib::RecognizerExt* recognizer = AxiomLib::trainFMDE(dataSet, env);

        return recognizer;
    } catch (AxiomLib::AxiomLibException e) {
        throw std::runtime_error("AxiomLibException: " + e.error());
    }
}

np::ndarray recognizer_get_distances(AxiomLib::RecognizerExt& self, np::ndarray& trajectory) {
    try {
        AxiomLib::MultiTS multiTS = fill_ts(trajectory);
        AxiomLib::TrajectorySampleDistance sampleDistances = self.getDistances(multiTS.data);

        py::tuple shape = py::make_tuple(sampleDistances.numClasses(), sampleDistances.length());
        np::ndarray distances = np::from_object(
                np::empty(shape, np::dtype::get_builtin<double>()),
                np::ndarray::C_CONTIGUOUS | np::ndarray::WRITEABLE
                );

        double* row_data = reinterpret_cast<double*>(distances.get_data());
        int noCols = sampleDistances.length();
        for (int row = 0; row < sampleDistances.numClasses(); ++row, row_data += noCols) {
            std::vector<double> dist_for_class;
            sampleDistances.getDistances(row, dist_for_class);
            ::memcpy(row_data, &dist_for_class[0], noCols * sizeof(double));
        }

        return distances;
    } catch (AxiomLib::AxiomLibException e) {
        throw std::runtime_error("AxiomLibException: " + e.error());
    }
}

const char* train_FMD_docstring = "env_dict: dict containing environment parameters\n"
        "dataset: Iterable of iterable of iterable of numpy.ndarray:\n"
        "for each dataset division (train, test, validation), for each class \n"
        "an iterable of class miltidimensional time series.\n"
        "each multidimensional time series is a C_CONTIGUOUS ndarray, where rows correspond to time series dimensions \n"
        "and columns correspond to time series values at a fixed point of time";

BOOST_PYTHON_MODULE(libaxiomlib_fmd)
{
    py::class_<AxiomLib::RecognizerExt>("recognizer")
            .def("get_distances", &recognizer_get_distances);

    py::def("train_FMD",
            &train_FMD,
            py::return_value_policy<py::manage_new_object>(),
            train_FMD_docstring,
            (py::arg("env_dict"), py::arg("dataset")));
}
