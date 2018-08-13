% % RM plots for big sample
tic;
close all;
load('average_harrison.mat');
load('average_smalley.mat');
load('pr_harrison.mat');
load('pr_smalley.mat');
load('mu_16.mat');
load('mu_8.mat');
load('mu_4.mat');
load('mu_2.mat');
har = good_carriers(pr_harrison);
mus = [];
mus(:,1) = get_exp(mu_2);
mus(:,2) = get_exp(mu_4);
mus(:,3) = get_exp(mu_8);
mus(:,4) = get_exp(mu_16);
max_dif = average_harrison - average_smalley;
% ratio = average_smalley./average_harrison;
ratio = mus;
ratio(:,1) = ratio(:,1)/2;
ratio(:,2) = ratio(:,2)/4;
ratio(:,3) = ratio(:,3)/8;
ratio(:,4) = ratio(:,4)/16;


mmax = 4;
imax = 0;
mmin = 1;
for j = mmin : mmax
    imax = imax + j;
end
for m = mmin : mmax
    for u = 1 : m
        
        weights = RMWeightHier(u,m,false);
        
        for dB = 250 : 292
%         for dB = 270 : 270
%             if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
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
                mu = ceil(ratio(dB-249,1) * n);
                eval(sprintf('codeInfo_%d_%d_%d.mu = %d;', dB , u, m, mu));
                
                %assigns H
                H = weights(1,mu+1);
                eval(sprintf('codeInfo_%d_%d_%d.H = %d;', dB , u, m, H));
                
                %assigns percentLeaked
                percentLeaked = 100*(k-H)/k;
                eval(sprintf('codeInfo_%d_%d_%d.percentLeaked = %d;', dB , u, m, percentLeaked));
                
                %assigns carrierRate
                carrierRate = average_harrison(dB-249,1)*rate;
                eval(sprintf('codeInfo_%d_%d_%d.carrierRate = %d;', dB , u, m, carrierRate));
                
%             end
        end
    end
end
% toc
% tic
for dB = 250 : 292
% for dB = 270 : 270
%     if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        workspace = who;
        eval(sprintf('outStr = regexpi(workspace, "codeInfo_%d_");', dB));
        ind = ~cellfun('isempty',outStr);
        eval(sprintf('vars%d = workspace(ind);', dB));
%     end
end
% toc
colors = jet(71);
counter = 0;
j = 0;
% tic

% ss = categorical(1,(imax-mmax)*32);
% rate = zeros(1,(imax-mmax)*32);
% percentLeaked = zeros(1,(imax-mmax)*32);
% ss_matched = categorical(1,32*mmax);
% rate_matched = zeros(1,32*mmax);
% percentLeaked_matched = zeros(1,32*mmax);
% figure();
hold on;
for dB = 250 : 292
% for dB = 270 : 270
%     if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        counter = counter + 1;
        match_count = 0;
        unmatch_count = 0;
        ss = categorical(1,imax-mmax);
        rate = zeros(1,imax-mmax);
        percentLeaked = zeros(1,imax-mmax);
        ss_matched = categorical(1,mmax);
        rate_matched = zeros(1,mmax);
        percentLeaked_matched = zeros(1,mmax);
%         pp = [];
%         figure(dB);
%         hold on;
        for i = 1:imax
            
            eval(sprintf('name = vars%d{%d,1};',dB,i));
            eval(sprintf('plotRate = %s.carrierRate;', name));
            eval(sprintf('plotPercentLeaked = 100 - %s.percentLeaked;', name));
            eval(sprintf('m = %s.m;', name));
            eval(sprintf('u = %s.u;', name));
            formatSpec = 'RM(%d,%d) %f1 dB';
            s = sprintf(formatSpec,u,m,dB/10);
%             ss{i,1} = s;
%             rate(1,i) = plotRate;
%             percentLeaked(1,i) = plotPercentLeaked;
            if m == u
%                 j = j + 1;
                match_count = match_count + 1;
                ss_matched(1,match_count) = categorical(cellstr(s));
                rate_matched(1,match_count) = plotRate;
                percentLeaked_matched(1,match_count) = plotPercentLeaked;
%                 scatter3(plotRate, plotPercentLeaked,categorical(cellstr(s)),144,colors(33-counter,:),'*');
            else
                unmatch_count = unmatch_count + 1;
                ss(1,unmatch_count) = categorical(cellstr(s));
                rate(1,unmatch_count) = plotRate;
                percentLeaked(1,unmatch_count) = plotPercentLeaked;
%                 scatter3(plotRate, plotPercentLeaked,categorical(cellstr(s)),[],colors(33-counter,:),'o');
            end
%             text(plotRate,plotPercentLeaked,num2str(i));
        end
        figure(1);
        hold on;
        scatter3(rate_matched,percentLeaked_matched,ss_matched,144,'black','*');
%         hold off;
%         figure(2);
%         hold on;
        scatter3(rate,percentLeaked,ss,[],colors(72-counter,:),'o');
        hold off;
        sss = categorical(ss);
%         scatter3(rate,percentLeaked,sss.',36,colors(33-counter,:),'o');
        

%     end
end
% scatter3(rate_matched,percentLeaked_matched,ss_matched,144,'*');
% scatter3(rate,percentLeaked,ss);
% sss = [ss.';ss_matched.'];
counter = 0;
% toc
% tic
rates = zeros(1,322);
ratios = zeros(1,322);
type = categorical(322);
i = 0;
figure(1);
hold on;
% for dB = 250 : 292
% % for dB = 270 : 270
% %     if max_har(dB-249,1) - max_har(dB-248,1) ~= 0
%         counter = counter + 1;
% %         scatter3(max_unique(dB-249,:),100,categorical(cellstr('No code ' + string(dB/10) + 'dB')),144,colors(33-counter,:),'s');
%         for index = 0 : average_sma(dB-249,1)
%             i = i + 1;
%             rates(1,i) = max_unique(dB-249,1) + index;
%             ratios(1,i) = (1 - index/(max_unique(dB-249)+index))*100;
%             type(1,i) = categorical(cellstr('No code ' + string(dB/10) + 'dB'));
%             scatter3(max_unique(dB-249,1) + index,(1 - index/(max_unique(dB-249)+index))*100, categorical(cellstr('No code ' + string(dB/10) + 'dB')),72,'g','d');
%         end
%         sss(end + 1,1) = cellstr('No code ' + string(dB/10));
% %     end
% end
% scatter3(rates,ratios,type,72,'g','d');
% set(gca,'zticklabel',sss)

title('Code Efficiency');
xlabel('Throughput Rate');
ylabel('Equivocation (%)');
zlabel('Reed-Muller Code (r,m)');
ylim([0 100]);
xlim([0 50]);
hold off
% grid on;
% hold off;
% figure(2);
% hold on;
% title('Code Efficiency');
% xlabel('Throughput Rate');
% ylabel('Equivocation (%)');
% zlabel('Reed-Muller Code (r,m)');
% ylim([0 100]);
% xlim([0 50]);
grid on;
hold off;
toc;
toc;