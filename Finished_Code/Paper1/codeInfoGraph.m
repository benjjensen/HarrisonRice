tic;
close all;

samePlot = true; %do you want all points on the same plot?
addpath('CollectedData/');
addpath('Functions/');
load('mm_har.mat');
load('mm_sma.mat');
load('harrisonOnlyCarriers.mat')

%save off the part of the array that is needed for calculations
max_har = mm_har(251:321,1);
max_sma = mm_smal(251:321,1);

%find ratio of carriers between harrison and smalley's offices
ratio = max_sma./max_har;

%iterate through all the codes
for m = 1 : 10
    for u = 1 : m
        
        %call function and save off the weigths of the codes
        weights = RMWeightHier(u,m,false);
        
        for dB = 250 : 312
            if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
                %creates object
                eval(sprintf('codeInfo_%d_%d_%d = codeInfo;', dB , u, m));
                
                %assigns u to the object
                eval(sprintf('codeInfo_%d_%d_%d.u = %d;', dB , u, m, u));
                
                %assigns m to the object
                eval(sprintf('codeInfo_%d_%d_%d.m = %d;', dB , u, m, m));
                
                %assigns n to the object
                n = 2^m;
                eval(sprintf('codeInfo_%d_%d_%d.n = %d;', dB , u, m, n));
                
                %assigns k to the object
                k = 0;
                for i = 0 : u
                    k = k + nchoosek(m,i);
                end
                eval(sprintf('codeInfo_%d_%d_%d.k = %d;', dB , u, m, k));
                
                %assigns rate to the object
                rate = k/n;
                eval(sprintf('codeInfo_%d_%d_%d.rate = %d;', dB , u, m, rate));
                
                %assigns dB to the object
                RdB = dB/10;
                eval(sprintf('codeInfo_%d_%d_%d.dBLevel = %d;', dB , u, m, RdB));
                
                %assigns mu to the object
                mu = ceil(ratio(dB-249,1) * n);
                eval(sprintf('codeInfo_%d_%d_%d.mu = %d;', dB , u, m, mu));
                
                %assigns H to the object
                H = weights(1,mu+1);
                eval(sprintf('codeInfo_%d_%d_%d.H = %d;', dB , u, m, H));
                
                %assigns percentLeaked to the object
                percentLeaked = 100*(k-H)/k;
                eval(sprintf('codeInfo_%d_%d_%d.percentLeaked = %d;', dB , u, m, percentLeaked));
                
                %assigns carrierRate to the object
                carrierRate = max_har(dB-249,1)*rate;
                eval(sprintf('codeInfo_%d_%d_%d.carrierRate = %d;', dB , u, m, carrierRate));
                
                %assignes percentH to the object so you can compare across codes
                percentH = 100 - percentLeaked;
                eval(sprintf('codeInfo_%d_%d_%d.percentH = %d;', dB , u, m, percentH));
                
            end
        end
    end
end

%calculates the number of codes at a given dB for a loop used later
codesPerdB = (m +1)*(m/2);

%find all the variables in the workspace that are of the same dB range so
%the colors on the plot match
for dB = 250 : 312
    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        workspace = who;
        eval(sprintf('outStr = regexpi(workspace, "codeInfo_%d_");', dB));
        ind = ~cellfun('isempty',outStr);
        eval(sprintf('vars%d = workspace(ind);', dB));
    end
end

%sets the color scheme
colors = jet(32);

%initialize counter
counter = 0;

%%%%All on same Plot%%%%
if (samePlot)
    figure();
    hold on;
    grid on;
end
%%%%%%%%%%%%%%%%%%%%%%%%
% istrue = false;

plotOnedB = zeros(1,320);
plotdB = zeros(1, 55);
plotIndex = zeros(1,55);



for dB = 250 : 312
    
    %checks to make sure the carriers used on the graphs are changing with
    %dB level
    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        counter = counter + 1;
        
        %%%All on different Plots%%%%
        if (~samePlot)
            figure(dB);
            hold on;
            grid on;
        end
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   
        if max_sma(dB-249,1) ~= 0
            clear extraExtra;
            clear plotextraExtra;
            clear extraCarriersH;
            clear extraCarriersRate;
        end
        
        %plot the
        for i = 1:codesPerdB
            eval(sprintf('name = vars%d{%d,1};',dB,i));
            
            searchName = name(14:end);
            [~, lengthName] = size(searchName);
            
            %%%All on different Plots%%%%
            if (~samePlot)
                counter2 = 0;
            end
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            
            %creates categories to find what code is what on the plot
            eval(sprintf('plotRate(i) = %s.carrierRate;', name));
            eval(sprintf('plotPercentH(i) = %s.percentH;', name));
            eval(sprintf('NameU = %s.u;', name));
            eval(sprintf('NameM = %s.m;', name));
            eval(sprintf('plotdB1(i) = "RM(%d,%d): %.1f dB";',NameU,NameM,dB/10));
            plotdB = categorical(plotdB1);
            plotIndex(i) = i;
            
            %%%All on different Plots%%%%
            if (~samePlot)
                scatter3(plotRate(i), plotPercentH(i), plotdB(i));
                
                if istrue
                    scatter3(plotOneRate, plotOnePercentH,plotOnedB,50,'k', '*');
                    istrue = false;
                end
                scatter(uncodedRate, uncodedSecrecy, [], 'r', 'd', 'DisplayName', 'Uncoded');
            end
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%
            
        end
        %%%All on same Plot%%%%
        if (samePlot)
            %plot all the points
            scatter3(plotRate, plotPercentH,plotdB,[],colors(33-counter,:),'DisplayName', sprintf('%.1f dB Limit', dB/10));
             %plot the best code in a black star (hard coded in)
            if (dB == 270)
                scatter3(21.75,100,plotdB(1),[],'k','*');
            end
        end
        %%%%%%%%%%%%%%%%%%%%%%%
        
        
        %%%All on different Plots%%%%
        if (~samePlot)
            dBTitle = round(double(dB/10),1);
            title(sprintf('Code Efficiency with %.1f dB Limit',dBTitle));
            xlabel('Throughput Rate');
            ylabel('Equivocation (%)');
            ylim([0 100]);
            xlim([0 50]);
            grid on;
            hold off;
        end
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    end
end
%save figure in certain format
saveas(gcf,'ThroughputGraph','epsc');
%%%All on same Plot%%%%
if (samePlot)
%%% make the graph look nice
    %title('Code Efficiency');
    %xlabel('Throughput Rate', 'FontSize', 12);
    xt = get(gca, 'XTick');
    set(gca, 'FontSize', 14)
    xlabel('bits per channel use', 'FontSize', 12);
    ylabel('Equivocation H(M|Z^n) (%)', 'FontSize', 12);
    zlabel('dB Level');
    ylim([0 100]);
    xlim([0 50]);
    %legend;
    saveas(gcf,'ThroughputGraph','epsc');
    hold off;
end
%%%%%%%%%%%%%%%%%%%%%%%
toc;