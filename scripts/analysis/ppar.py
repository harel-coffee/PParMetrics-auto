#! /usr/bin/python3

if __name__ != "__main__":
    # prepare a set of metrics for further reference
    metrics = {}
    metrics['loop-proportion'] = ['loop-absolute-size', 
                                  'loop-payload-fraction', 
                                  'loop-proper-sccs-number']
    metrics['iterator-payload-cohesion'] = ['iterator-payload-total-cohesion', 
                                            'iterator-payload-non-cf-cohesion', 
                                            'iterator-payload-mem-cohesion']
    metrics['critical-payload-cohesion'] = ['critical-payload-total-cohesion', 
                                            'critical-payload-non-cf-cohesion', 
                                            'critical-payload-mem-cohesion']
    metrics['payload-dependencies-number'] = ['payload-total-dependencies-number', 
                                              'payload-true-dependencies-number', 
                                              'payload-anti-dependencies-number', 
                                              'payload-output-dependencies-number']
    metrics['critical-payload-dependencies-number'] = ['critical-payload-total-dependencies-number', 
                                                       'critical-payload-true-dependencies-number', 
                                                       'critical-payload-anti-dependencies-number'] 
    metrics['all'] = []
    for metric_group_list in metrics:
        if metric_group_list != 'all':
            metrics['all'].extend(metrics[metric_group_list])
    metric_list = metrics['all'] 
