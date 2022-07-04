#ifndef TS_CPP_BACKENDS_CORE_BACKEND_HH_
#define TS_CPP_BACKENDS_CORE_BACKEND_HH_

#include <fmt/format.h>
#include <memory>
#include <stdexcept>
#include <utility>

#include "src/utils/config.hh"
#include "src/utils/message.hh"
#include "src/utils/model_archive.hh"

namespace torchserve {
    // TorchServe ModelInstance Interface
  class ModelInstance {
    public:
    ModelInstance() {};
    ~ModelInstance() {};

    virtual std::shared_ptr<torchserve::InferenceResponse> Predict(
      std::unique_ptr<torchserve::InferenceRequest> inference_request) = 0;

    protected:
    // instance_id naming convention:
    // device_type + ":" + device_id (or object id)
    std::string instance_id_;
    std::shared_ptr<torchserve::LoadModelRequest> load_model_request_;
  };

  /**
   * Note:
   * Any framework should implement its own backend which includes:
   * 1. Implement class Backend
   * 2. Implement class ModelInstance
   * 3. function std::shared_ptr<Backend> CreateBackend()
   * 
   * The core idea:
   * - A framework has its own backend in a model server.
   * - A backend has multiple model instances.
   * - A worker is associated with one model instance.
   * - A model instance is one model loaded on CPU or GPU.
   */

  // TorchServe Backend Interface
  class Backend {
    public:
    enum ModelInstanceStatus {
      NOT_INIT,
      INIT,
      READY
    };

    Backend() {};
    ~Backend() {};

    std::pair<std::unique_ptr<torchserve::LoadModelResponse>, std::shared_ptr<ModelInstance>> 
    LoadModel(std::shared_ptr<torchserve::LoadModelRequest> load_model_request);

    virtual 
    std::pair<std::unique_ptr<torchserve::LoadModelResponse>, std::shared_ptr<ModelInstance>> 
    LoadModelInternal(std::shared_ptr<torchserve::LoadModelRequest> load_model_request) = 0;

    protected:
    // key: model_instance_id
    // value: model_instance_status
    std::map<std::string, torchserve::Backend::ModelInstanceStatus> model_instance_status;

    // key: model_instance_id
    // value: model_instance    
    std::map<std::string, std::shared_ptr<torchserve::ModelInstance>> model_instance_table;
  };

  class ModelWorker {
    public:
    ModelWorker() {};
    ~ModelWorker();

    private:
    std::shared_ptr<torchserve::ModelInstance> model_instance_;
  };
}  // namespace torchserve
#endif // TS_CPP_BACKENDS_CORE_BACKEND_HH_