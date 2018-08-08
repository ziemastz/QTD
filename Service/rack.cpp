#include "rack.h"

Rack::Rack(QVariantList _tab, int module, int channel, QObject *parent) : QObject(parent)
{
    max = _tab.count();
    tab = _tab;
    foreach(QVariant var, tab) {
        switch(module) {
        case HV_MODULE:
            switch (channel) {
            case CH_HVANODEBETA:
                tags << "A:"+var.toString()+"V";
                break;
            case CH_HVFIRSTDYNODEBETA:
                tags << "D1:"+var.toString()+"V";
                break;
            case CH_HVFOCUSINGDYNODEBETA:
                tags << "F:"+var.toString()+"V";
                break;
            case CH_HVANODEGAMMA:
                tags << "G:"+var.toString()+"V";
                break;
            default:
                break;
            }
            break;
        case ANALYZER_MODULE:
            switch (channel) {
            case CH_RESOLVINGTIMEBETA:
                tags << "R1:"+var.toString()+"ns";
                break;
            case CH_RESOLVINGTIMEGAMMA:
                tags << "R2:"+var.toString()+"ns";
                break;
            case CH_DEADTIMEBETA:
                tags << "DT1:"+var.toString()+"ns";
                break;
            case CH_DEADTIMEGAMMA:
                tags << "DT2:"+var.toString()+"ns";
                break;
            case CH_DELAYTIMEBETA:
                tags << "DLB:"+var.toString()+"ns";
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }


    }
    curr = 0;
    changed = true;
    mod = module;
    ch = channel;
}

Rack::~Rack()
{

}

void Rack::move()
{
    int next = curr+1;
    if(next>max)
        next = 1;
    if(next != curr)
        changed = true;
    curr = next;
}

bool Rack::isChanged()
{
    if(changed){
        changed = false;
        return true;
    }else
        return false;
}

void Rack::apply()
{
    if(isChanged()){
        switch (mod) {
        case HV_MODULE:
        {
            emit setVolt(tab.at(curr-1).toFloat(),ch);
            break;
        }
        case ANALYZER_MODULE:
        {
            emit setTime(tab.at(curr-1).toInt(),ch);
            break;
        }
        default:
            break;
        }
    }
}
