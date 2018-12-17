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
    # metric values before LLVM opt optimizations
    metric_groups['loop-proportion_before'] = ['loop-absolute-size_before', 
                                        'loop-payload-fraction_before', 
                                        'loop-proper-sccs-number_before',
                                        'loop-critical-payload-fraction_before']
    metric_groups['iterator-payload-cohesion_before'] = ['iterator-payload-total-cohesion_before', 
                                                  'iterator-payload-non-cf-cohesion_before']
    metric_groups['critical-payload-cohesion_before'] = ['critical-payload-total-cohesion_before', 
                                                  'critical-payload-non-cf-cohesion_before']
    metric_groups['payload-dependencies-number_before'] = ['payload-total-dependencies-number_before', 
                                                    'payload-true-dependencies-number_before', 
                                                    'payload-anti-dependencies-number_before', 
                                                    'payload-output-dependencies-number_before', 
                                                    'payload-cross-dependencies-number_before'] 
    metric_groups['critical-payload-dependencies-number_before'] = ['critical-payload-total-dependencies-number_before', 
                                                             'critical-payload-true-dependencies-number_before', 
                                                             'critical-payload-anti-dependencies-number_before',
                                                             'critical-payload-output-dependencies-number_before',
                                                             'critical-payload-cross-dependencies-number_before']
    metric_groups['iterator-nature_before'] = ['iterator-mem-write-count_before',
                                        'iterator-mem-write-fraction_before',
                                        'iterator-call-count_before',
                                        'iterator-call-fraction_before']
    metric_groups['payload-nature_before'] = ['payload-mem-write-count_before',
                                       'payload-mem-write-fraction_before',
                                       'payload-call-count_before',
                                       'payload-call-fraction_before']
    metric_groups['critical-payload-nature_before'] = ['critical-payload-mem-write-count_before',
                                                'critical-payload-mem-write-fraction_before',
                                                'critical-payload-call-count_before',
                                                'critical-payload-call-fraction_before']
    # metric values after LLVM opt optimizations
    metric_groups['loop-proportion_after'] = ['loop-absolute-size_after', 
                                        'loop-payload-fraction_after', 
                                        'loop-proper-sccs-number_after',
                                        'loop-critical-payload-fraction_after']
    metric_groups['iterator-payload-cohesion_after'] = ['iterator-payload-total-cohesion_after', 
                                                  'iterator-payload-non-cf-cohesion_after']
    metric_groups['critical-payload-cohesion_after'] = ['critical-payload-total-cohesion_after', 
                                                  'critical-payload-non-cf-cohesion_after']
    metric_groups['payload-dependencies-number_after'] = ['payload-total-dependencies-number_after', 
                                                    'payload-true-dependencies-number_after', 
                                                    'payload-anti-dependencies-number_after', 
                                                    'payload-output-dependencies-number_after', 
                                                    'payload-cross-dependencies-number_after'] 
    metric_groups['critical-payload-dependencies-number_after'] = ['critical-payload-total-dependencies-number_after', 
                                                             'critical-payload-true-dependencies-number_after', 
                                                             'critical-payload-anti-dependencies-number_after',
                                                             'critical-payload-output-dependencies-number_after',
                                                             'critical-payload-cross-dependencies-number_after']
    metric_groups['iterator-nature_after'] = ['iterator-mem-write-count_after',
                                        'iterator-mem-write-fraction_after',
                                        'iterator-call-count_after',
                                        'iterator-call-fraction_after']
    metric_groups['payload-nature_after'] = ['payload-mem-write-count_after',
                                       'payload-mem-write-fraction_after',
                                       'payload-call-count_after',
                                       'payload-call-fraction_after']
    metric_groups['critical-payload-nature_after'] = ['critical-payload-mem-write-count_after',
                                                'critical-payload-mem-write-fraction_after',
                                                'critical-payload-call-count_after',
                                                'critical-payload-call-fraction_after']

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
