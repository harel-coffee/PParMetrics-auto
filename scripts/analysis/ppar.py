#! /usr/bin/python3

def is_outlier(sample, data, std_num):
    if std_num == 0:
        return False

    if (abs(sample - data.mean()) > std_num*data.std()):
        return True
    else:
        return False

def create_folder(directory):
    try:
        if not os.path.exists(directory):
            os.makedirs(directory)
    except OSError:
        print ("error: could not create directory: " + directory)

if __name__ != "__main__":

    # prepare a set of metrics for further reference
    # each metric group contains a list of its metrics
    metric_groups = {}
    metric_groups['loop-proportion'] = ['loop-absolute-size', 
                                        'loop-payload-fraction', 
                                        'loop-proper-sccs-number',
                                        'loop-critical-payload-fraction']
    metric_groups['iterator-payload-cohesion'] = ['iterator-payload-total-cohesion', 
                                                  'iterator-payload-non-cf-cohesion']
    metric_groups['critical-payload-cohesion'] = ['critical-payload-total-cohesion', 
                                                  'critical-payload-non-cf-cohesion']
    metric_groups['payload-dependencies-number'] = ['payload-total-dependencies-number', 
                                                    'payload-true-dependencies-number', 
                                                    'payload-anti-dependencies-number', 
                                                    'payload-output-dependencies-number', 
                                                    'payload-cross-dependencies-number'] 
    metric_groups['critical-payload-dependencies-number'] = ['critical-payload-total-dependencies-number', 
                                                             'critical-payload-true-dependencies-number', 
                                                             'critical-payload-anti-dependencies-number',
                                                             'critical-payload-output-dependencies-number',
                                                             'critical-payload-cross-dependencies-number']
    metric_groups['iterator-nature'] = ['iterator-mem-write-count',
                                        'iterator-mem-write-fraction',
                                        'iterator-call-count',
                                        'iterator-call-fraction']
    metric_groups['payload-nature'] = ['payload-mem-write-count',
                                       'payload-mem-write-fraction',
                                       'payload-call-count',
                                       'payload-call-fraction']
    metric_groups['critical-payload-nature'] = ['critical-payload-mem-write-count',
                                                'critical-payload-mem-write-fraction',
                                                'critical-payload-call-count',
                                                'critical-payload-call-fraction']
    metric_groups['all'] = []
    for metric_group_name in metric_groups:
        if metric_group_name != 'all':
            metric_groups['all'].extend(metric_groups[metric_group_name])
    metric_list = metric_groups['all']

    # prepare a set of metrics for further reference
    # each metric group contains a list of its metrics
    metric_sets = {}
    
    # set_0 - all metrics altogether
    metric_sets['set_0'] = metric_groups['all']
    
    # set_1 - metrics, correlating with parallelizability
    metric_sets['set_1'] = ['iterator-payload-total-cohesion', 
                            'iterator-payload-non-cf-cohesion',
                            'loop-critical-payload-fraction',
                            'loop-payload-fraction']
    
    # set_2 - everything, but dependencies number metrics
    metric_sets['set_2'] = []
    metric_sets['set_2'].extend(metric_groups['loop-proportion'])
    metric_sets['set_2'].extend(metric_groups['iterator-payload-cohesion'])
    metric_sets['set_2'].extend(metric_groups['critical-payload-cohesion'])
    metric_sets['set_2'].extend(metric_groups['iterator-nature'])
    metric_sets['set_2'].extend(metric_groups['payload-nature'])
    metric_sets['set_2'].extend(metric_groups['critical-payload-nature'])

    # set_3 - everything, but cohesion number metrics
    metric_sets['set_3'] = []
    metric_sets['set_3'].extend(metric_groups['loop-proportion'])
    metric_sets['set_3'].extend(metric_groups['payload-dependencies-number'])
    metric_sets['set_3'].extend(metric_groups['critical-payload-dependencies-number'])
    metric_sets['set_3'].extend(metric_groups['iterator-nature'])
    metric_sets['set_3'].extend(metric_groups['payload-nature'])
    metric_sets['set_3'].extend(metric_groups['critical-payload-nature'])
   
    # set_4 - everything, but proportion metrics
    metric_sets['set_4'] = []
    metric_sets['set_4'].extend(metric_groups['iterator-payload-cohesion'])
    metric_sets['set_4'].extend(metric_groups['critical-payload-cohesion'])
    metric_sets['set_4'].extend(metric_groups['payload-dependencies-number'])
    metric_sets['set_4'].extend(metric_groups['critical-payload-dependencies-number'])
    metric_sets['set_4'].extend(metric_groups['iterator-nature'])
    metric_sets['set_4'].extend(metric_groups['payload-nature'])
    metric_sets['set_4'].extend(metric_groups['critical-payload-nature'])

    # set_5 - everything, but nature metrics
    metric_sets['set_5'] = []
    metric_sets['set_5'].extend(metric_groups['loop-proportion'])
    metric_sets['set_5'].extend(metric_groups['iterator-payload-cohesion'])
    metric_sets['set_5'].extend(metric_groups['critical-payload-cohesion'])
    metric_sets['set_5'].extend(metric_groups['payload-dependencies-number'])
    metric_sets['set_5'].extend(metric_groups['critical-payload-dependencies-number'])
