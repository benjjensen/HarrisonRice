% % RM plots for big sample
tic;
close all;
load('pr_harrison.mat');
load('pr_smalley.mat');
threshold1 = .99;
threshold2 = .99999;
decibel = false; % to color code based off of decibel vs block length
for bl = 0:17
    eval(sprintf('load("mu_%d_2");',bl));
end
num_carriers = get_num_carrier(pr_harrison,threshold1);
mus = [];
for bl = 0:17
    eval(sprintf('mus(:,%d) = get_worst_case(mu_%d_2,threshold2);',bl+1,bl));
end
% ratio = mus;
[col_num,~] = size(mu_2_2);
col_num;


mmax = 17;
imax = 0;
mmin = 0;
for j = mmin : mmax
    imax = imax + j;
end
for m = mmin : mmax
    for u = 1 : m
        weights = RMWeightHier(u,m,false);
        
        for dB = 250 : 281
                eval(sprintf('mu_pmf = mu_%d_2(%d,:);',m,dB-249));

                %creates object
                eval(sprintf('codeInfo_%d_%d_%d = codeInfo;', dB , u, m));
                
                %assigns u
                eval(sprintf('codeInfo_%d_%d_%d.u = %d;', dB , u, m, u));
                
                %assigns m
                eval(sprintf('codeInfo_%d_%d_%d.m = %d;', dB , u, m, m));
                
                %assigns n
                n = 2^m;
                eval(sprintf('codeInfo_%d_%d_%d.n = %d;', dB , u, m, n));
                
                %assigns k
                k = 0;
                for i = 0 : u
                    k = k + nchoosek(m,i);
                end
                eval(sprintf('codeInfo_%d_%d_%d.k = %d;', dB , u, m, k));
                
                %assigns rate
                rate = k/n;
                eval(sprintf('codeInfo_%d_%d_%d.rate = %d;', dB , u, m, rate));
                
                %assigns dB
                RdB = dB/10;
                eval(sprintf('codeInfo_%d_%d_%d.dBLevel = %d;', dB , u, m, RdB));
                
                %assigns mu
                mu = mus(dB-249,m+1);
                eval(sprintf('codeInfo_%d_%d_%d.mu = %d;', dB , u, m, mu));
                
                %assigns H
%                 H = weights(1,mu+1);
                [H,percentLeaked] = getExpectedEquivocation(mu_pmf,k,weights);
                eval(sprintf('codeInfo_%d_%d_%d.H = %d;', dB , u, m, H));
                
                %assigns percentLeaked
%                 percentLeaked = 100*(k-H)/k;
                eval(sprintf('codeInfo_%d_%d_%d.percentLeaked = %d;', dB , u, m, percentLeaked));
                
                %assigns carrierRate
                carrierRate = num_carriers(dB-249,1)*rate;
                eval(sprintf('codeInfo_%d_%d_%d.carrierRate = %d;', dB , u, m, carrierRate));                
        end
    end
end
if decibel == true
    for dB = 250 : 281
        workspace = who;
        eval(sprintf('outStr = regexpi(workspace, "codeInfo_%d_");', dB));
        ind = ~cellfun('isempty',outStr);
        eval(sprintf('vars%d = workspace(ind);', dB));
    end
    colors = jet(col_num+1);
    counter = 0;
    j = 0;
    hold on;
    for dB = 250 : 281
        counter = counter + 1;
        match_count = 0;
        unmatch_count = 0;
        ss = categorical(1,imax-mmax);
        rate = zeros(1,imax-mmax);
        percentLeaked = zeros(1,imax-mmax);
        ss_matched = categorical(1,mmax);
        rate_matched = zeros(1,mmax);
        percentLeaked_matched = zeros(1,mmax);
        for i = 1:imax
            
            eval(sprintf('name = vars%d{%d,1};',dB,i));
            eval(sprintf('plotRate = %s.carrierRate;', name));
            eval(sprintf('plotPercentLeaked = 100 - %s.percentLeaked;', name));
            eval(sprintf('m = %s.m;', name));
            eval(sprintf('u = %s.u;', name));
            formatSpec = 'RM(%d,%d) %f1 dB';
            s = sprintf(formatSpec,u,m,dB/10);
            if m == u
                match_count = match_count + 1;
                ss_matched(1,match_count) = categorical(cellstr(s));
                rate_matched(1,match_count) = plotRate;
                percentLeaked_matched(1,match_count) = plotPercentLeaked;
            else
                unmatch_count = unmatch_count + 1;
                ss(1,unmatch_count) = categorical(cellstr(s));
                rate(1,unmatch_count) = plotRate;
                percentLeaked(1,unmatch_count) = plotPercentLeaked;
            end
        end
        figure(1);
        hold on;
        scatter3(rate_matched,percentLeaked_matched,ss_matched,144,'black','*');
        scatter3(rate,percentLeaked,ss,[],colors(col_num+1-counter,:),'o');
        hold off;
    end
else
    for m = 1:mmax
        workspace = who;
        str = sprintf("%scodeInfo_%s_%s_%d%s","\w*","\d*","\d*",m,"\>");
        outStr = regexpi(workspace, str);
        ind = ~cellfun('isempty',outStr);
        eval(sprintf('vars%d = workspace(ind);', m));
    end
    colors = jet(mmax+1);
    counter = 0;
    j = 0;
    hold on;
    for m = 1 : mmax
        imax = 32*m;
        counter = counter + 1;
        match_count = 0;
        unmatch_count = 0;
        ss = categorical(1,imax-col_num);
        rate = zeros(1,imax-col_num);
        percentLeaked = zeros(1,imax-col_num);
        ss_matched = categorical(1,col_num);
        rate_matched = zeros(1,col_num);
        percentLeaked_matched = zeros(1,col_num);
        for i = 1:32*m
            
            eval(sprintf('name = vars%d{%d,1};',m,i));
            eval(sprintf('plotRate = %s.carrierRate;', name));
            eval(sprintf('plotPercentLeaked = floor(100 - %s.percentLeaked);', name));
            eval(sprintf('m = %s.m;', name));
            eval(sprintf('u = %s.u;', name));
            eval(sprintf('dB = %s.dBLevel;', name));
            formatSpec = 'RM(%d,%d) %f1 dB';
            s = sprintf(formatSpec,u,m,dB);
            if m == u
                match_count = match_count + 1;
                ss_matched(1,match_count) = categorical(cellstr(s));
                rate_matched(1,match_count) = plotRate;
                percentLeaked_matched(1,match_count) = plotPercentLeaked;
            else
                unmatch_count = unmatch_count + 1;
                ss(1,unmatch_count) = categorical(cellstr(s));
                rate(1,unmatch_count) = plotRate;
                percentLeaked(1,unmatch_count) = plotPercentLeaked;
            end
        end
        figure(1);
        hold on;
        scatter3(rate_matched,percentLeaked_matched,ss_matched,144,'black','*');
        if(m~=1)
            scatter3(rate,percentLeaked,ss,[],colors(m,:),'o');
        end
        hold off;
    end
end


figure(1);
hold on;


title('Code Efficiency');
xlabel('Throughput Rate');
ylabel('Equivocation (%)');
zlabel('Reed-Muller Code (r,m)');
% ylim([90 100]);
% xlim([0 20]);
grid on;
hold off
toc;
