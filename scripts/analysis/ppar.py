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
    metric_groups['dynamic'] = ['loop-abs-time', 
                                'loop-app-frac-time', 
                                'loop-abs-time-self',
                                'loop-app-frac-time-self',
                                'loop-entries-num',
                                'loop-exits-num',
                                'loop-min-iter-num',
                                'loop-avg-iter-num',
                                'loop-max-iter-num']
    metric_groups['loop-proportion'] = ['loop-absolute-size', 
                                        'loop-iterator-size', 
                                        'loop-payload-fraction', 
                                        'loop-proper-sccs-number',
                                        'loop-proper-sccs-number-diff',
                                        'loop-proper-sccs-ddg-number',
                                        'loop-proper-sccs-ddg-number-diff',
                                        'loop-critical-payload-fraction',
                                        'loop-depth',
                                        'loop-inner-loop-num']
    metric_groups['loop-proportion-0'] = ['loop-absolute-size', 
                                          'loop-iterator-size', 
                                          'loop-payload-fraction'] 
    metric_groups['loop-proportion-1'] = ['loop-iterator-size', 
                                        'loop-payload-fraction', 
                                        'loop-proper-sccs-number',
                                        'loop-depth',
                                        'loop-inner-loop-num']
    metric_groups['loop-proportion-2'] = ['loop-absolute-size', 
                                        'loop-payload-fraction', 
                                        'loop-critical-payload-fraction']

    metric_groups['iterator-payload-cohesion'] = ['iterator-payload-total-cohesion', 
                                                  'iterator-payload-non-cf-cohesion']
    metric_groups['critical-payload-cohesion'] = ['critical-payload-total-cohesion', 
                                                  'critical-payload-non-cf-cohesion']
    metric_groups['iterator-dependencies-number'] = ['iterator-total-dependencies-number', 
                                                    'iterator-true-dependencies-number', 
                                                    'iterator-anti-dependencies-number', 
                                                    'iterator-output-dependencies-number', 
                                                    'iterator-cross-dependencies-number', 
                                                    'iterator-conf-dependencies-number', 
                                                    'iterator-reg-dependencies-number', 
                                                    'iterator-mem-dependencies-number',
                                                    'iterator-cntl-dependencies-number']
    metric_groups['iterator-dependencies-number-0'] = ['iterator-total-dependencies-number', 
                                                       'iterator-true-dependencies-number', 
                                                       'iterator-anti-dependencies-number', 
                                                       'iterator-output-dependencies-number', 
                                                       'iterator-cross-dependencies-number', 
                                                       'iterator-conf-dependencies-number', 
                                                       'iterator-reg-dependencies-number', 
                                                       'iterator-mem-dependencies-number',
                                                       'iterator-cntl-dependencies-number'] 
    metric_groups['payload-dependencies-number'] = ['payload-total-dependencies-number', 
                                                    'payload-true-dependencies-number', 
                                                    'payload-anti-dependencies-number', 
                                                    'payload-output-dependencies-number', 
                                                    'payload-cross-dependencies-number', 
                                                    'payload-conf-dependencies-number', 
                                                    'payload-reg-dependencies-number', 
                                                    'payload-mem-dependencies-number', 
#                                                    'payload-mem-dependencies-number-diff', 
                                                    'payload-cntl-dependencies-number']
    metric_groups['payload-dependencies-number-0'] = ['payload-cross-dependencies-number', 
                                                      'payload-conf-dependencies-number', 
                                                      'payload-mem-dependencies-number'] 
    metric_groups['critical-payload-dependencies-number'] = ['critical-payload-total-dependencies-number', 
                                                             'critical-payload-true-dependencies-number', 
                                                             'critical-payload-anti-dependencies-number',
                                                             'critical-payload-output-dependencies-number',
                                                             'critical-payload-cross-dependencies-number',
                                                             'critical-payload-conf-dependencies-number',
                                                             'critical-payload-reg-dependencies-number',
                                                             'critical-payload-mem-dependencies-number',
                                                             'critical-payload-cntl-dependencies-number']
    metric_groups['critical-payload-dependencies-number-0'] = ['critical-payload-total-dependencies-number', 
                                                             'critical-payload-true-dependencies-number', 
                                                             'critical-payload-anti-dependencies-number',
                                                             'critical-payload-output-dependencies-number',
                                                             'critical-payload-cross-dependencies-number',
                                                             'critical-payload-conf-dependencies-number',
                                                             'critical-payload-reg-dependencies-number',
                                                             'critical-payload-mem-dependencies-number',
                                                             'critical-payload-cntl-dependencies-number']
    metric_groups['iterator-nature'] = ['iterator-mem-write-count',
                                        'iterator-mem-write-fraction',
                                        'iterator-mem-read-count',
                                        'iterator-mem-read-fraction',
                                        'iterator-call-count',
                                        'iterator-call-fraction',
                                        'iterator-branch-count',
                                        'iterator-branch-fraction',
                                        'iterator-getelemptr-count',
                                        'iterator-getelemptr-fraction']
    metric_groups['iterator-nature-no-count'] = ['iterator-mem-write-fraction',
                                                 'iterator-mem-read-fraction',
                                                 'iterator-call-fraction',
                                                 'iterator-branch-fraction',
                                                 'iterator-getelemptr-fraction']
    metric_groups['iterator-nature-no-frac'] = ['iterator-mem-write-count',
                                                'iterator-mem-read-count',
                                                'iterator-call-count',
                                                'iterator-branch-count',
                                                'iterator-getelemptr-count']
    metric_groups['payload-nature'] = ['payload-mem-write-count',
                                       'payload-mem-write-fraction',
                                       'payload-mem-read-count',
                                       'payload-mem-read-fraction',
                                       'payload-call-count',
                                       'payload-call-fraction',
                                       'payload-branch-count',
                                       'payload-branch-fraction',
                                       'payload-getelemptr-count',
                                       'payload-getelemptr-fraction']
    metric_groups['payload-nature-no-count'] = ['payload-mem-write-fraction',
                                                'payload-mem-read-fraction',
                                                'payload-call-fraction',
                                                'payload-branch-fraction',
                                                'payload-getelemptr-fraction']
    metric_groups['payload-nature-no-frac'] = ['payload-mem-write-count',
                                               'payload-mem-read-count',
                                               'payload-call-count',
                                               'payload-branch-count',
                                               'payload-getelemptr-count']
    metric_groups['critical-payload-nature'] = ['critical-payload-mem-write-count',
                                                'critical-payload-mem-write-fraction',
                                                'critical-payload-mem-read-count',
                                                'critical-payload-mem-read-fraction',
                                                'critical-payload-call-count',
                                                'critical-payload-call-fraction',
                                                'critical-payload-branch-count',
                                                'critical-payload-branch-fraction',
                                                'critical-payload-getelemptr-count',
                                                'critical-payload-getelemptr-fraction']
    metric_groups['critical-payload-nature-no-count'] = ['critical-payload-mem-write-fraction',
                                                         'critical-payload-mem-read-fraction',
                                                         'critical-payload-call-fraction',
                                                         'critical-payload-branch-fraction',
                                                         'critical-payload-getelemptr-fraction']
    metric_groups['critical-payload-nature-no-frac'] = ['critical-payload-mem-write-count',
                                                        'critical-payload-mem-read-count',
                                                        'critical-payload-call-count',
                                                        'critical-payload-branch-count',
                                                        'critical-payload-getelemptr-count']

    metric_groups['all'] = []
    for metric_group_name in metric_groups:
        if metric_group_name != 'all':
            metric_groups['all'].extend(metric_groups[metric_group_name])
    metric_groups['all'] = list(set(metric_groups['all']))
    metric_list = metric_groups['all']

    # prepare a set of metrics for further reference
    # each metric group contains a list of its metrics
    metric_sets = {}
    
    # set all - all metrics altogether
    metric_sets['all'] = metric_groups['all']

    # set all, but cohesion
    metric_sets['all_but_cohes'] = []
    metric_sets['all_but_cohes'].extend(metric_groups['loop-proportion'])
    metric_sets['all_but_cohes'].extend(metric_groups['iterator-dependencies-number'])
    metric_sets['all_but_cohes'].extend(metric_groups['payload-dependencies-number'])
    metric_sets['all_but_cohes'].extend(metric_groups['critical-payload-dependencies-number'])
    metric_sets['all_but_cohes'].extend(metric_groups['iterator-nature'])
    metric_sets['all_but_cohes'].extend(metric_groups['payload-nature'])
    metric_sets['all_but_cohes'].extend(metric_groups['critical-payload-nature'])

    # set all, but dependencies - everything, but dependencies number metrics
    metric_sets['all_but_dep'] = []
    metric_sets['all_but_dep'].extend(metric_groups['loop-proportion'])
    metric_sets['all_but_dep'].extend(metric_groups['iterator-payload-cohesion'])
    metric_sets['all_but_dep'].extend(metric_groups['critical-payload-cohesion'])
    metric_sets['all_but_dep'].extend(metric_groups['iterator-nature'])
    metric_sets['all_but_dep'].extend(metric_groups['payload-nature'])
    metric_sets['all_but_dep'].extend(metric_groups['critical-payload-nature'])

    # set all, but proportions - everything, but proportion metrics
    metric_sets['all_but_prop'] = []
    metric_sets['all_but_prop'].extend(metric_groups['iterator-payload-cohesion'])
    metric_sets['all_but_prop'].extend(metric_groups['critical-payload-cohesion'])
    metric_sets['all_but_prop'].extend(metric_groups['iterator-dependencies-number'])
    metric_sets['all_but_prop'].extend(metric_groups['payload-dependencies-number'])
    metric_sets['all_but_prop'].extend(metric_groups['critical-payload-dependencies-number'])
    metric_sets['all_but_prop'].extend(metric_groups['iterator-nature-no-count'])
    metric_sets['all_but_prop'].extend(metric_groups['payload-nature'])
    metric_sets['all_but_prop'].extend(metric_groups['critical-payload-nature'])

    # set all, but nature - everything, but nature metrics
    metric_sets['all_but_nature'] = []
    metric_sets['all_but_nature'].extend(metric_groups['loop-proportion'])
    metric_sets['all_but_nature'].extend(metric_groups['iterator-payload-cohesion'])
    metric_sets['all_but_nature'].extend(metric_groups['critical-payload-cohesion'])
    metric_sets['all_but_nature'].extend(metric_groups['iterator-dependencies-number'])
    metric_sets['all_but_nature'].extend(metric_groups['payload-dependencies-number'])
    metric_sets['all_but_nature'].extend(metric_groups['critical-payload-dependencies-number'])
    
    # set [ all, but call count related features ]
    metric_sets['no_call'] = metric_groups['all'][:]
    call_exclude_list = ['iterator-call-count', 
                         'iterator-call-fraction', 
                         'payload-call-count', 
                         'payload-call-fraction', 
                         'critical-payload-call-count', 
                         'critical-payload-call-fraction']
    for exclude in call_exclude_list:
        if exclude in metric_sets['no_call']: 
            metric_sets['no_call'].remove(exclude)

    # set [ all, but call count related features ]
    metric_sets['no_alias'] = metric_groups['all'][:]
    alias_exclude_list = ['iterator-conf-dependencies-number',
                          'payload-conf-dependencies-number',
                          'critical-payload-conf-dependencies-number']
    for exclude in alias_exclude_list:
        if exclude in metric_sets['no_alias']: 
            metric_sets['no_alias'].remove(exclude)
    
    # set [ all, but call count related features ]
    metric_sets['no_call_no_alias'] = metric_groups['all'][:]
    exclude_list = call_exclude_list
    exclude_list.extend(alias_exclude_list)
    for exclude in exclude_list:
        if exclude in metric_sets['no_call_no_alias']: 
            metric_sets['no_call_no_alias'].remove(exclude)

    # set [ all, but call and alias count related features ]
    # set for OpenMP version of labels
    metric_sets['omp'] = metric_groups['all'][:]
    exclude_list = ['iterator-call-count', 
                    'iterator-call-fraction', 
                    'payload-call-count', 
                    'payload-call-fraction', 
                    'critical-payload-call-count', 
                    'critical-payload-call-fraction']
    for exclude in exclude_list:
        if exclude in metric_sets['omp']: 
            metric_sets['omp'].remove(exclude)

    # set [ all, but call and alias count related features ]
    # set for OpenMP version of labels
    metric_sets['no_frac'] = metric_groups['all'][:]
    exclude_list = ['loop-payload-fraction', 
                    'loop-critical-payload-fraction', 
                    'iterator-mem-write-fraction',
                    'iterator-mem-read-fraction',
                    'iterator-call-fraction', 
                    'iterator-branch-fraction', 
                    'iterator-getelemptr-fraction',
                    'payload-mem-write-fraction',
                    'payload-mem-read-fraction',
                    'payload-call-fraction', 
                    'payload-branch-fraction', 
                    'payload-getelemptr-fraction',
                    'critical-payload-mem-write-fraction',
                    'critical-payload-mem-read-fraction',
                    'critical-payload-call-fraction', 
                    'critical-payload-branch-fraction', 
                    'critical-payload-getelemptr-fraction']
    for exclude in exclude_list:
        if exclude in metric_sets['no_frac']: 
            metric_sets['no_frac'].remove(exclude)

    metric_sets['no_count'] = metric_groups['all'][:]
    exclude_list = ['loop-proper-sccs-number',
                    'loop-proper-sccs-number-diff',
                    'loop-proper-sccs-ddg-number',
                    'loop-proper-sccs-ddg-number-diff',
                    'iterator-mem-write-count',
                    'iterator-mem-read-count',
                    'iterator-call-count', 
                    'iterator-branch-count', 
                    'iterator-getelemptr-count',
                    'payload-mem-write-count',
                    'payload-mem-read-count',
                    'payload-call-count', 
                    'payload-branch-count', 
                    'payload-getelemptr-count',
                    'critical-payload-mem-write-count',
                    'critical-payload-mem-read-count',
                    'critical-payload-call-count', 
                    'critical-payload-branch-count', 
                    'critical-payload-getelemptr-count']
    for exclude in exclude_list:
        if exclude in metric_sets['no_count']: 
            metric_sets['no_count'].remove(exclude)

    metric_sets['precise_all'] = []
    metric_sets['precise_all'].extend(['loop-absolute-size', 
                                       'loop-iterator-size', 
                                       'loop-payload-fraction', 
                                       'loop-proper-sccs-number',
                                       'loop-proper-sccs-number-diff',
                                       'loop-proper-sccs-ddg-number',
                                       'loop-proper-sccs-ddg-number-diff',
                                       'loop-critical-payload-fraction',
                                       'loop-depth',
                                       'loop-inner-loop-num'])
    metric_sets['precise_all'].extend(['iterator-payload-total-cohesion', 
                                       'iterator-payload-non-cf-cohesion'])
    metric_sets['precise_all'].extend(['critical-payload-total-cohesion', 
                                       'critical-payload-non-cf-cohesion'])
    metric_sets['precise_all'].extend(['iterator-total-dependencies-number', 
                                       'iterator-true-dependencies-number', 
                                       'iterator-anti-dependencies-number', 
                                       'iterator-output-dependencies-number', 
                                       'iterator-cross-dependencies-number', 
                                       'iterator-conf-dependencies-number', 
                                       'iterator-reg-dependencies-number', 
                                       'iterator-mem-dependencies-number',
                                       'iterator-cntl-dependencies-number'])
    metric_sets['precise_all'].extend(['payload-total-dependencies-number', 
                                       'payload-true-dependencies-number', 
                                       'payload-anti-dependencies-number', 
                                       'payload-output-dependencies-number', 
                                       'payload-cross-dependencies-number', 
                                       'payload-conf-dependencies-number', 
                                       'payload-reg-dependencies-number', 
                                       'payload-mem-dependencies-number', 
#                                       'payload-mem-dependencies-number-diff', 
                                       'payload-cntl-dependencies-number'])
    metric_sets['precise_all'].extend(['critical-payload-total-dependencies-number', 
                                       'critical-payload-true-dependencies-number', 
                                       'critical-payload-anti-dependencies-number',
                                       'critical-payload-output-dependencies-number',
                                       'critical-payload-cross-dependencies-number',
                                       'critical-payload-conf-dependencies-number',
                                       'critical-payload-reg-dependencies-number',
                                       'critical-payload-mem-dependencies-number',
                                       'critical-payload-cntl-dependencies-number'])
    metric_sets['precise_all'].extend(['iterator-mem-write-count',
                                       'iterator-mem-write-fraction',
                                       'iterator-mem-read-count',
                                       'iterator-mem-read-fraction',
                                       'iterator-call-count',
                                       'iterator-call-fraction',
                                       'iterator-branch-count',
                                       'iterator-branch-fraction',
                                       'iterator-getelemptr-count',
                                       'iterator-getelemptr-fraction'])
    metric_sets['precise_all'].extend(['payload-mem-write-count',
                                       'payload-mem-write-fraction',
                                       'payload-mem-read-count',
                                       'payload-mem-read-fraction',
                                       'payload-call-count',
                                       'payload-call-fraction',
                                       'payload-branch-count',
                                       'payload-branch-fraction',
                                       'payload-getelemptr-count',
                                       'payload-getelemptr-fraction'])
    metric_sets['precise_all'].extend(['critical-payload-mem-write-count',
                                       'critical-payload-mem-write-fraction',
                                       'critical-payload-mem-read-count',
                                       'critical-payload-mem-read-fraction',
                                       'critical-payload-call-count',
                                       'critical-payload-call-fraction',
                                       'critical-payload-branch-count',
                                       'critical-payload-branch-fraction',
                                       'critical-payload-getelemptr-count',
                                       'critical-payload-getelemptr-fraction'])
    # precise 0
    metric_sets['precise_0'] = []
    metric_sets['precise_0'].extend(['loop-iterator-size', 
                                     'loop-payload-fraction', 
                                     'loop-proper-sccs-number-diff',
                                     'loop-proper-sccs-ddg-number',
                                     'loop-proper-sccs-ddg-number-diff',
                                     'loop-inner-loop-num'])
    metric_sets['precise_0'].extend(['payload-total-dependencies-number', 
                                     'payload-true-dependencies-number', 
                                     'payload-anti-dependencies-number', 
                                     'payload-output-dependencies-number', 
                                     'payload-cross-dependencies-number', 
                                     'payload-conf-dependencies-number', 
                                     'payload-reg-dependencies-number', 
                                     'payload-mem-dependencies-number', 
#                                     'payload-mem-dependencies-number-diff', 
                                     'payload-cntl-dependencies-number'])
    metric_sets['precise_0'].extend(['critical-payload-total-dependencies-number', 
                                     'critical-payload-true-dependencies-number', 
                                     'critical-payload-anti-dependencies-number',
                                     'critical-payload-output-dependencies-number',
                                     'critical-payload-cross-dependencies-number',
                                     'critical-payload-conf-dependencies-number',
                                     'critical-payload-reg-dependencies-number',
                                     'critical-payload-mem-dependencies-number',
                                     'critical-payload-cntl-dependencies-number'])
    metric_sets['precise_0'].extend(['iterator-mem-write-count',
                                     'iterator-mem-read-count',
                                     'iterator-call-count',
                                     'iterator-branch-count',
                                     'iterator-getelemptr-count'])
    metric_sets['precise_0'].extend(['payload-mem-write-count',
                                     'payload-mem-read-count',
                                     'payload-call-count',
                                     'payload-branch-count',
                                     'payload-getelemptr-count'])
    metric_sets['precise_0'].extend(['critical-payload-mem-write-count',
                                     'critical-payload-mem-read-count',
                                     'critical-payload-call-count',
                                     'critical-payload-branch-count',
                                     'critical-payload-getelemptr-count'])
