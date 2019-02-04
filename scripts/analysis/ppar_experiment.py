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
    metric_groups['loop-proportion-pre-pre'] = ['loop-absolute-size-pre', 
                                        'loop-iterator-size-pre', 
                                        'loop-payload-fraction-pre', 
                                        'loop-proper-sccs-number-pre',
                                        'loop-proper-sccs-number-diff-pre',
                                        'loop-critical-payload-fraction-pre',
                                        'loop-depth-pre',
                                        'loop-inner-loop-num-pre']
    metric_groups['iterator-payload-cohesion-pre'] = ['iterator-payload-total-cohesion-pre', 
                                                  'iterator-payload-non-cf-cohesion-pre']
    metric_groups['critical-payload-cohesion-pre'] = ['critical-payload-total-cohesion-pre', 
                                                  'critical-payload-non-cf-cohesion-pre']
    metric_groups['iterator-dependencies-number-pre'] = ['iterator-total-dependencies-number-pre', 
                                                    'iterator-true-dependencies-number-pre', 
                                                    'iterator-anti-dependencies-number-pre', 
                                                    'iterator-output-dependencies-number-pre', 
                                                    'iterator-cross-dependencies-number-pre', 
                                                    'iterator-conf-dependencies-number-pre', 
                                                    'iterator-reg-dependencies-number-pre', 
                                                    'iterator-mem-dependencies-number-pre',
                                                    'iterator-cntl-dependencies-number-pre']
    metric_groups['payload-dependencies-number-pre'] = ['payload-total-dependencies-number-pre', 
                                                    'payload-true-dependencies-number-pre', 
                                                    'payload-anti-dependencies-number-pre', 
                                                    'payload-output-dependencies-number-pre', 
                                                    'payload-cross-dependencies-number-pre', 
                                                    'payload-conf-dependencies-number-pre', 
                                                    'payload-reg-dependencies-number-pre', 
                                                    'payload-mem-dependencies-number-pre', 
                                                    'payload-mem-dependencies-number-diff-pre', 
                                                    'payload-cntl-dependencies-number-pre']
    metric_groups['critical-payload-dependencies-number-pre'] = ['critical-payload-total-dependencies-number-pre', 
                                                             'critical-payload-true-dependencies-number-pre', 
                                                             'critical-payload-anti-dependencies-number-pre',
                                                             'critical-payload-output-dependencies-number-pre',
                                                             'critical-payload-cross-dependencies-number-pre',
                                                             'critical-payload-conf-dependencies-number-pre',
                                                             'critical-payload-reg-dependencies-number-pre',
                                                             'critical-payload-mem-dependencies-number-pre',
                                                             'critical-payload-cntl-dependencies-number-pre']
    metric_groups['iterator-nature-pre'] = ['iterator-mem-write-count-pre',
                                        'iterator-mem-write-fraction-pre',
                                        'iterator-mem-read-count-pre',
                                        'iterator-mem-read-fraction-pre',
                                        'iterator-call-count-pre',
                                        'iterator-call-fraction-pre',
                                        'iterator-branch-count-pre',
                                        'iterator-branch-fraction-pre',
                                        'iterator-getelemptr-count-pre',
                                        'iterator-getelemptr-fraction-pre']
    metric_groups['payload-nature-pre'] = ['payload-mem-write-count-pre',
                                       'payload-mem-write-fraction-pre',
                                       'payload-mem-read-count-pre',
                                       'payload-mem-read-fraction-pre',
                                       'payload-call-count-pre',
                                       'payload-call-fraction-pre',
                                       'payload-branch-count-pre',
                                       'payload-branch-fraction-pre',
                                       'payload-getelemptr-count-pre',
                                       'payload-getelemptr-fraction-pre']
    metric_groups['critical-payload-nature-pre'] = ['critical-payload-mem-write-count-pre',
                                                'critical-payload-mem-write-fraction-pre',
                                                'critical-payload-mem-read-count-pre',
                                                'critical-payload-mem-read-fraction-pre',
                                                'critical-payload-call-count-pre',
                                                'critical-payload-call-fraction-pre',
                                                'critical-payload-branch-count-pre',
                                                'critical-payload-branch-fraction-pre',
                                                'critical-payload-getelemptr-count-pre',
                                                'critical-payload-getelemptr-fraction-pre']

    metric_groups['loop-proportion-after'] = ['loop-absolute-size-after', 
                                        'loop-iterator-size-after', 
                                        'loop-payload-fraction-after', 
                                        'loop-proper-sccs-number-after',
                                        'loop-proper-sccs-number-diff-after',
                                        'loop-critical-payload-fraction-after',
                                        'loop-depth-after',
                                        'loop-inner-loop-num-after']
    metric_groups['iterator-payload-cohesion-after'] = ['iterator-payload-total-cohesion-after', 
                                                  'iterator-payload-non-cf-cohesion-after']
    metric_groups['critical-payload-cohesion-after'] = ['critical-payload-total-cohesion-after', 
                                                  'critical-payload-non-cf-cohesion-after']
    metric_groups['iterator-dependencies-number-after'] = ['iterator-total-dependencies-number-after', 
                                                    'iterator-true-dependencies-number-after', 
                                                    'iterator-anti-dependencies-number-after', 
                                                    'iterator-output-dependencies-number-after', 
                                                    'iterator-cross-dependencies-number-after', 
                                                    'iterator-conf-dependencies-number-after', 
                                                    'iterator-reg-dependencies-number-after', 
                                                    'iterator-mem-dependencies-number-after',
                                                    'iterator-cntl-dependencies-number-after']
    metric_groups['payload-dependencies-number-after'] = ['payload-total-dependencies-number-after', 
                                                    'payload-true-dependencies-number-pre', 
                                                    'payload-anti-dependencies-number-after', 
                                                    'payload-output-dependencies-number-after', 
                                                    'payload-cross-dependencies-number-after', 
                                                    'payload-conf-dependencies-number-after', 
                                                    'payload-reg-dependencies-number-after', 
                                                    'payload-mem-dependencies-number-after', 
                                                    'payload-mem-dependencies-number-diff-after', 
                                                    'payload-cntl-dependencies-number-after']
    metric_groups['critical-payload-dependencies-number-after'] = ['critical-payload-total-dependencies-number-after', 
                                                             'critical-payload-true-dependencies-number-after', 
                                                             'critical-payload-anti-dependencies-number-after',
                                                             'critical-payload-output-dependencies-number-after',
                                                             'critical-payload-cross-dependencies-number-after',
                                                             'critical-payload-conf-dependencies-number-after',
                                                             'critical-payload-reg-dependencies-number-after',
                                                             'critical-payload-mem-dependencies-number-after',
                                                             'critical-payload-cntl-dependencies-number-after']
    metric_groups['iterator-nature-after'] = ['iterator-mem-write-count-after',
                                        'iterator-mem-write-fraction-after',
                                        'iterator-mem-read-count-after',
                                        'iterator-mem-read-fraction-after',
                                        'iterator-call-count-after',
                                        'iterator-call-fraction-after',
                                        'iterator-branch-count-after',
                                        'iterator-branch-fraction-after',
                                        'iterator-getelemptr-count-after',
                                        'iterator-getelemptr-fraction-after']
    metric_groups['payload-nature-after'] = ['payload-mem-write-count-after',
                                       'payload-mem-write-fraction-after',
                                       'payload-mem-read-count-after',
                                       'payload-mem-read-fraction-after',
                                       'payload-call-count-after',
                                       'payload-call-fraction-after',
                                       'payload-branch-count-after',
                                       'payload-branch-fraction-after',
                                       'payload-getelemptr-count-after',
                                       'payload-getelemptr-fraction-after']
    metric_groups['critical-payload-nature-after'] = ['critical-payload-mem-write-count-after',
                                                'critical-payload-mem-write-fraction-after',
                                                'critical-payload-mem-read-count-after',
                                                'critical-payload-mem-read-fraction-after',
                                                'critical-payload-call-count-after',
                                                'critical-payload-call-fraction-after',
                                                'critical-payload-branch-count-after',
                                                'critical-payload-branch-fraction-after',
                                                'critical-payload-getelemptr-count-after',
                                                'critical-payload-getelemptr-fraction-after']


    metric_groups['all'] = []
    for metric_group_name in metric_groups:
        if metric_group_name != 'all':
            metric_groups['all'].extend(metric_groups[metric_group_name])
    metric_groups['all'] = list(set(metric_groups['all']))
    metric_list = metric_groups['all']

    # prepare a set of metrics for further reference
    # each metric group contains a list of its metrics
    metric_sets = {}
    
    # set_0 - all metrics altogether
    metric_sets['set_0'] = metric_groups['all']
    
