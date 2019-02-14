#! /usr/bin/python3

def check_ml_pipeline_config(pl_cfg):
   
    # check that all sections present in the config file
    mandatory_sections = [
            'preprocess',
            'feature_selection',
            'model_selection',
            'hyper_param_selection',
            'model_training',
            'model_validation',
            'report']
    
    for section in mandatory_sections:
        if section not in pl_cfg:
            print ("ML pipeline config: format error: missing " + section + " section")
            return False
    
    return True

if __name__ != "__main__":

    available_scalers = ['std','min_max','max_abs']
    available_models = ['SVC','DT','MLP','RFC','AdaBoost']
    available_feature_selectors = ['SelectFromModel','SelectKBest']

    # machine learning pipeline consists of several conceptual stages;
    # each one taking over after the other
    config = {}
    config['preprocess'] = {}
    config['feature_selection'] = {}
    config['model_selection'] = {}
    config['hyper_param_selection'] = {}
    config['model_training'] = {}
    config['model_validation'] = {}
    config['report'] = {}

    config['preprocess']['scaler'] = ['std','min_max','max_abs']
    config['preprocess']['center'] = True
    config['preprocess']['scale'] = True
    
    config['feature_selection']['auto'] = True
    config['feature_selection']['method'] = ['SelectFromModel']
    config['feature_selection']['median'] = 1
    config['feature_selection']['model'] = available_models

    config['model_selection']['model'] = available_models
    
    config['hyper_param_selection']['auto'] = True
    config['hyper_param_selection']['model'] = available_models
    
    config['model_training']['type'] = ['tt','k_fold']

    config['model_validation']['type'] = ['tt','k_fold']

    config['report']['dummy'] = True
