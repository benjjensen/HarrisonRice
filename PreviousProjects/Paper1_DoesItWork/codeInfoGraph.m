tic;
% close all;
clear;
samePlot = true; %do you want all points on the same plot?
% addpath('CollectedData/');
% addpath('Functions/');
load('Data/cap_har.mat'); % this data was calculated using functions from paper2
load('Data/eve.mat');

%save off the part of the array that is needed for calculations
cap_har = cap_har/2;
eve = eve/2;
%find ratio of carriers between harrison and smalley's offices
ratio = eve./cap_har;
num_index = 300;
dB = logspace(-.2,.7,num_index);
dB = 10*log10(dB);
codesPerdB = 0;
%iterate through all the codes
for m = 1 : 6
    for u = 1 : m
        %call function and save off the weigths of the codes
        weights = RMWeightHier(u,m,false);
        codesPerdB = codesPerdB + 1;
        for index = 1:num_index - 1
            if ((cap_har(index,1)- cap_har(index+1,1) ~= 0 && ratio(index) <= 1) || ...
                (eve(index,1)- eve(index+1,1) ~= 0 && ratio(index) <= 1))
                %creates object
                eval(sprintf('codeInfo_%d_%d_%d = codeInfo;', index , u, m));
                
                %assigns u to the object
                eval(sprintf('codeInfo_%d_%d_%d.u = %d;', index , u, m, u));
                
                %assigns m to the object
                eval(sprintf('codeInfo_%d_%d_%d.m = %d;', index , u, m, m));
                
                %assigns n to the object
                n = 2^m;
                eval(sprintf('codeInfo_%d_%d_%d.n = %d;', index , u, m, n));
                
                %assigns k to the object
                k = 0;
                for i = 0 : u
                    k = k + nchoosek(m,i);
                end
                eval(sprintf('codeInfo_%d_%d_%d.k = %d;', index , u, m, k));
                
                %assigns rate to the object
                rate = k/n;
                eval(sprintf('codeInfo_%d_%d_%d.rate = %d;', index , u, m, rate));
                
                %assigns dB to the object
                RdB = dB(index);
                eval(sprintf('codeInfo_%d_%d_%d.dBLevel = %f;', index , u, m, RdB));

                %assigns mu to the object
                mu = ceil(ratio(index) * n);
                if mu > n
                    mu = n;
                end
                eval(sprintf('codeInfo_%d_%d_%d.mu = %d;', index , u, m, mu));
                
                %assigns H to the object
                H = weights(1,mu+1);    % +1 to account for index shift 
                eval(sprintf('codeInfo_%d_%d_%d.H = %d;', index , u, m, H));
                
                %assigns percentLeaked to the object
                percentLeaked = 100*(k-H)/k;
                eval(sprintf('codeInfo_%d_%d_%d.percentLeaked = %d;', index , u, m, percentLeaked));
                
                %assigns carrierRate to the object
                carrierRate = cap_har(index)*rate;
                eval(sprintf('codeInfo_%d_%d_%d.carrierRate = %d;', index , u, m, carrierRate));
                
                %assignes percentH to the object so you can compare across codes
                percentH = 100 - percentLeaked;
                eval(sprintf('codeInfo_%d_%d_%d.percentH = %d;', index , u, m, percentH));
                
            end
        end
    end
end


%find all the variables in the workspace that are of the same dB range so
%the colors on the plot match
for index = 1:num_index - 1
    if ((cap_har(index,1)- cap_har(index+1,1) ~= 0 && ratio(index) <= 1) || ...
        (eve(index,1)- eve(index+1,1) ~= 0 && ratio(index) <= 1))
        workspace = who;
        eval(sprintf('outStr = regexpi(workspace, "codeInfo_%d_");', index));
        ind = ~cellfun('isempty',outStr);
        eval(sprintf('vars%d = workspace(ind);', index));
    end
end

%sets the color scheme
colors = jet(66);

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



for index = 1:num_index - 1
    
    %checks to make sure the carriers used on the graphs are changing with
    %dB level
    if ((cap_har(index,1)- cap_har(index+1,1) ~= 0 && ratio(index) <= 1) || ...
        (eve(index,1)- eve(index+1,1) ~= 0 && ratio(index) <= 1))
        counter = counter + 1;
        
        %%%All on different Plots%%%%
        if (~samePlot)
            figure(dB);
            hold on;
            grid on;
        end
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   
        if eve(index) ~= 0
            clear extraExtra;
            clear plotextraExtra;
            clear extraCarriersH;
            clear extraCarriersRate;
        end
        
        %plot the
        for i = 1:codesPerdB
            eval(sprintf('name = vars%d{%d,1};',index,i));
            
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
            eval(sprintf('plotdB1(i) = "RM(%d,%d): %.1f dB";',NameU,NameM,dB(index)));
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
            scatter3(plotRate, plotPercentH,plotdB,[],colors(counter,:),'DisplayName', sprintf('%.1f dB Limit', dB(index)));
             %plot the best code in a black star (hard coded in)
            if index == 167
                scatter3(19,100,plotdB(1),[],'k','*');
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
    set(gca, 'FontName', 'Times New Roman', 'FontSize', 14)
%     xlabel('    ');
%     ylabel('    ');
    xlabel('Throughput (bits per channel use)', 'FontName', 'Times New Roman', 'FontSize', 12);
    ylabel('Equivocation $H(M|Z^n)$ (\%)', 'FontName', 'Times New Roman', 'FontSize', 12, 'interpreter', 'latex');
    zlabel('dB Level');
    ylim([0 100]);
    xlim([0 50]);
    %legend;
    scatter(19,100,100,'k','*', 'LineWidth',.5);
%     saveas(gcf,'ThroughputGraph','epsc');
    hold off;
    
    axis = gca;
    axis.FontName = 'Times New Roman';
    ff = gcf;
    homer = ff.Units;
    ff.Units = 'inches';
    bart = ff.Position;
    ff.Position = [bart(1:2) 5 4];
    ff.PaperPositionMode = 'auto';
    ff.Units = homer;
end
%%%%%%%%%%%%%%%%%%%%%%%
toc;