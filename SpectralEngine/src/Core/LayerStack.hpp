//
//  LayerStack.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 20.03.24.
//

#include "pch.h"
#include "Layer.hpp"

namespace Spectral {
    
    class LayerStack
    {
    public:
        
        LayerStack();
        ~LayerStack();
        
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* overlay);
        
        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }
        
    private:
        
        std::vector<Layer*> m_Layers;
        std::vector<Layer*>::iterator m_LayerInsert;
    };
}
