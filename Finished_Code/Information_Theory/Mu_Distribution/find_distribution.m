load('pr_harrison.mat');
load('pr_smalley.mat');

threshold = .99;
gc = good_carriers(pr_harrison, threshold);
tic;
for i = 16:16
    disp(i);
    block_length = 2^i;
    eval(sprintf( ...
        'mu_%d_2 = bl_distribution_avg(gc,pr_smalley,block_length);'...
        , i));
%     eval(sprintf('save("mu_%d_2","mu_%d_2");',i,i));
end
toc;
