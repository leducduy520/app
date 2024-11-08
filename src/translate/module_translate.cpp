#include "module_translate.hpp"
#include "thread_pool.hpp"

using namespace std;
REGISTER_MODULE_CLASS(ModuleTranslator, NAME)

ModuleTranslator::ModuleTranslator()
    :ModuleInterface(NAME)
{}

void ModuleTranslator::execute()
{
    m_result = ThreadPool::getInstance()->submit(100, [&](){
        // Translate the message
        cout << m_moduleName << " translate message\n";
        // Simulate translation delay
        this_thread::sleep_for(chrono::seconds(5));
        cout << m_moduleName << " translation finished\n";
    });
}

void ModuleTranslator::shutdown()
{
    if(m_result.valid())
    {
        m_result.get();
        cout << m_moduleName << " translation finished\n";
    }
}
