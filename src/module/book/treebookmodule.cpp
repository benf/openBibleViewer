#include "treebookmodule.h"

TreeBookModule::TreeBookModule() : m_tree(NULL)
{
}
TreeBookModule::~TreeBookModule()
{
    if(m_tree != NULL) {
        delete m_tree;
        m_tree = NULL;
    }
}

void TreeBookModule::setSettings(Settings *settings)
{
    m_settings = settings;
}