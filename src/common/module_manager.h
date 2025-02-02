#ifndef __MODULE_MANAGER_H__
#define __MODULE_MANAGER_H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>
#include <future>
#include <type_traits>
#include <optional>
#include <atomic>

#ifdef _WIN32
#include <Windows.h>
typedef HMODULE LibraryHandle;
typedef FARPROC FunctionAddress;
#define LoadLibraryFunction LoadLibrary
#define GetFunctionAddress GetProcAddress
#define UnloadLibrary FreeLibrary
#else
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>
typedef void* LibraryHandle;
typedef void* FunctionAddress;
#define LoadLibraryFunction(path) dlopen(path, RTLD_LAZY)
#define GetFunctionAddress dlsym
#define UnloadLibrary dlclose
#endif

#if defined(_MSC_VER)
#undef max
#endif
namespace dld
{
    class ModuleInterface
    {
    protected:
        const std::string m_moduleName;
        std::atomic<bool> m_running;

    public:
        ModuleInterface(std::string moduleName);

        virtual ~ModuleInterface();

        virtual void execute() = 0;
        virtual void shutdown() = 0;
    };

    template <typename key_type = std::string, typename instance_type = void*>
    class ModuleFactory
    {
        std::unordered_map<key_type, std::function<std::shared_ptr<instance_type>(void)>> m_module_constructor;
        std::unordered_map<key_type, std::shared_ptr<instance_type>> m_module_instance;

    public:
        std::shared_ptr<instance_type> getInstance(const key_type& moduleName)
        {
            auto fit = m_module_constructor.find(moduleName);
            if (fit != m_module_constructor.end())
            {
                if (auto search = m_module_instance.find(moduleName); search == m_module_instance.end())
                {
                    auto module = fit->second();
                    m_module_instance[moduleName] = module;
                }
                return m_module_instance[moduleName];
            }
            return nullptr;
        }

        void registerModule(const key_type& moduleName, std::function<std::shared_ptr<instance_type>(void)> createFunc)
        {
            m_module_constructor[moduleName] = createFunc;
        }

        void releaseInstance(const key_type& moduleName)
        {
            auto search = m_module_instance.find(moduleName);
            if (search != m_module_instance.end())
            {
                if constexpr (std::is_same<instance_type, ModuleInterface>::value)
                {
                    search->second->shutdown();
                }
                m_module_instance.erase(search);
            }
        }

        void releaseConstructor(const key_type& moduleName)
        {
            auto search = m_module_constructor.find(moduleName);
            if (search != m_module_constructor.end())
            {
                m_module_constructor.erase(search);
            }
        }

        void release(const key_type& moduleName)
        {
            releaseInstance(moduleName);
            releaseConstructor(moduleName);
        }

        void release()
        {
            if constexpr (std::is_same<instance_type, ModuleInterface>::value)
            {
                for (auto& instance : m_module_instance)
                {
                    instance.second->shutdown();
                }
            }
            m_module_instance.clear();
            m_module_constructor.clear();
        }
    };

    class ModuleManager
    {
    public:
        // Register a module with its expected path

        void registerModule(const std::string& moduleName, const std::string& modulePath);
        template <class T, class Y>
        void registerModuleConstructor(T&& moduleName, Y&& constructor);

        // Load a registered module
        bool loadModule(const std::string& moduleName);

        // Get a function pointer from a loaded module
        FunctionAddress getModuleMethod(const std::string& moduleName, const std::string& functionName);

        // Release a loaded module
        void releaseModuleLib(const std::string& moduleName);
        void releaseModuleInstance(const std::string& moduleName);

        // Get the actual path of a loaded module
        std::string getModulePath(const std::string& moduleName);

        std::shared_ptr<ModuleInterface> getModuleInstance(const std::string& moduleName);

        static ModuleManager* getInstance();

        void endSession();

        // Destructor to ensure all modules are released
        ~ModuleManager();
        ModuleManager() = default;

        friend class ModuleFactory<std::string, ModuleInterface>;
        friend class ModuleInterface;

    private:
        std::unordered_map<std::string, std::string> m_module_paths;
        std::unordered_map<std::string, LibraryHandle> m_module_handle;
        ModuleFactory<std::string, ModuleInterface> m_factory;
    };

    template <typename T>
    class ModuleRegistar
    {
    public:
        ModuleRegistar(std::string moduleName)
        {
            ModuleManager::getInstance()->registerModuleConstructor(
                moduleName,
                []() -> std::shared_ptr<ModuleInterface> { return std::make_shared<T>(); });
        }
    };

#if defined(_WIN32)
#define REGISTER_MODULE_LOCATION(moduleName, modulePath)                                                               \
    ModuleManager::getInstance()->registerModule(#moduleName, std::string(#modulePath).append(".dll"));
#else
#define REGISTER_MODULE_LOCATION(moduleName, modulePath)                                                               \
    ModuleManager::getInstance()->registerModule(#moduleName, std::string(#modulePath).append(".so"));
#endif

#define REGISTER_MODULE_CLASS(moduleclass, moduleName)                                                                 \
    static const ModuleRegistar<moduleclass> register##moduleclass(moduleName);

    template <class T, class Y>
    inline void ModuleManager::registerModuleConstructor(T&& moduleName, Y&& constructor)
    {
        ModuleManager::getInstance()->m_factory.registerModule(std::forward<T>(moduleName),
                                                               std::forward<Y>(constructor));
    }
} // namespace dld
#endif // __MODULE_MANAGER_H__
