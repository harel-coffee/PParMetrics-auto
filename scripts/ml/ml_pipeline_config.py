#! /usr/bin/python3

# sections of INI configuration file correspond to 
# different ML pipeline stages
mandatory_sections = [
    'header',
    'preprocess',
    'feature_selection',
    'model_selection',
    'hyper_param_selection',
    'model_training',
    'model_testing',
    'report'
]
    
available_scalers = [
    'raw',
    'std',
    'min_max',
    'max_abs'
]

available_models = [
    'SVC',
    'DT',
    'MLP',
    'RFC',
    'AdaBoost'
]

available_baselines = [
    'stratified',
    'most_frequent',
    'prior',
    'uniform',
    'constant'
]

available_feature_selectors = [
    'VarianceThreshold',
    'SelectFromModel',
    'SelectKBest'
]

univariate_feature_tests = [
    'mutual_info_classif',
    'f_classif',
    'chi2'
]

def check_ml_pipeline_config(pl_cfg):
   
    # check that all sections present in the config file
    for section in mandatory_sections:
        if section not in pl_cfg:
            print ("ML pipeline config: format error: missing " + section + " section")
            return False
   
    hp_cfg = pl_cfg['hyper_param_selection']
    train_cfg = pl_cfg['model_training']
    test_cfg = pl_cfg['model_testing']
    header_cfg = pl_cfg['header']

    if hp_cfg['model'] != train_cfg['model'] or\
       train_cfg['model'] != test_cfg['model']:
        return False

    if header_cfg['type'] not in ['tt','kfold']:
        sys.exit("error: configuration file check: header has an unrecognised type: " + header_cfg['type'])

    return True

if __name__ != "__main__":

    # machine learning pipeline consists of several conceptual stages;
    # each one taking over after the other
    config = {}
    config['header'] = {}
    config['preprocess'] = {}
    config['feature_selection'] = {}
    config['model_selection'] = {}
    config['hyper_param_selection'] = {}
    config['model_training'] = {}
    config['model_testing'] = {}
    config['report'] = {}

    # #
    # preprocessing stage configuration parameters
    # #
    config['preprocess']['scaler'] = available_scalers
    config['preprocess']['center'] = True
    config['preprocess']['scale'] = True
 
    # #
    # preprocessing stage configuration parameters
    # #
    config['feature_selection']['auto'] = True # do automatic feature selection
    config['feature_selection']['methods'] = available_feature_selectors
    config['feature_selection']['medians'] = 1
    config['feature_selection']['thresholds'] = 0.0
    config['feature_selection']['models'] = available_models

    config['model_selection']['model'] = available_models
    
    config['hyper_param_selection']['auto'] = True
    config['hyper_param_selection']['model'] = available_models
    
    config['model_training']['type'] = ['tt','k_fold']

    config['model_testing']['type'] = ['tt','k_fold']

    config['report']['dummy'] = True
