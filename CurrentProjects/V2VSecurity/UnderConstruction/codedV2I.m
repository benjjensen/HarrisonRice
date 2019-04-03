%% Created by Bradford Clark
% March 6, 2019
% This file creates figures that show at what position Eve gets a given
% revealed number of bits and where bob can recieve all of the given bits

%% Actual File
close all
addpath('../Functions');
addpath('../Data');
load('dataForCodedCases.mat');

syms x;

for k = 50:50
    
    maxNumberOfCarriers = 32;
    carriersPerLocation = cap_alpha(1:end,k);
    numberOfSpots = length(carriersPerLocation);
    
    %find scale values for Eves and bobs graphs
    maxPosition = 435.2;
    scaleEve = double(solve(maxPosition == x * numberOfSpots,x));
    scaleBob = double(solve(maxPosition == x * numberOfSpots*2,x));
    midPosition = 229.7;
    
    
    
    %find RM weights
    u = 2;
    m = 5;
    kBits = 0;
    for i = 0 : u
        kBits = kBits + nchoosek(m,i);
    end
    RMWeights = RMWeightHier(u,m,false);
    
    %set bobs knowledge to maxNumberOfCarriers only if he gets all carriers
    bobsKnowledge = zeros(numberOfSpots*2);
    for i = 1:numberOfSpots
        if carriersPerLocation(i) == maxNumberOfCarriers
            bobsKnowledge(:,2*i-1:2*i) = maxNumberOfCarriers;
        else
            bobsKnowledge(:,2*i-1:2*i) = NaN;
        end
    end
    
    %find Eves Equivocation matrix
    EvesEquivocation = zeros(numberOfSpots);
    
    for i = 1:numberOfSpots
        EvesEquivocation(i,:) = RMWeights(1+(carriersPerLocation(i)));
    end
    
    %create axis for graphs
    [Evey Evex] = size(EvesEquivocation);
    xaxis = zeros(Evex);
    yaxis = zeros(Evey);
    for i = 1:32
        xaxis(:,i) = i;
        yaxis(i,:) = i;
    end
    
    %search for cases where bob gets all 32 bits
    placesBobGetsAll = [];
    placesBobGetsAll = strfind(bobsKnowledge(1,:),maxNumberOfCarriers)';
    
    %find vertices for the hatches
    countIndex = 1;
    placesBobGetsAllArea = [];
    for i = 1:length(placesBobGetsAll)
        if ((i == 1) &&...
                (placesBobGetsAll(i) + 1 == placesBobGetsAll(i+1)))
            placesBobGetsAllArea(countIndex) = placesBobGetsAll(i);
            countIndex = countIndex + 1;
        elseif ((i == length(placesBobGetsAll)) &&...
                (placesBobGetsAll(i) - 1 == placesBobGetsAll(i-1)))
            placesBobGetsAllArea(countIndex) = placesBobGetsAll(i);
            countIndex = countIndex + 1;
        elseif (i ~= 1 && i ~= length(placesBobGetsAll)) &&...
                xor((placesBobGetsAll(i-1) == placesBobGetsAll(i) - 1),...
                (placesBobGetsAll(i+1) == placesBobGetsAll(i) + 1))
            placesBobGetsAllArea(countIndex) = placesBobGetsAll(i);
            countIndex = countIndex + 1;
        end
        
    end
    
    %% plot Bobs and Eves things
    figure(100+k);
    hold on;
    
    %create colormap
    myColorMapScatter = flipud(gray(kBits+1));
    colormap(myColorMapScatter);
    
    %find the xy axis
    xyaxis = 0:scaleEve:Evex * scaleEve - scaleEve;
    
    [minValue,closestIndex] = min(abs(xyaxis-midPosition));
    closestValue = xyaxis(closestIndex);
    xyaxis = xyaxis-closestValue;
    
    %plot Eves equivocation
    Eve = pcolor(xyaxis,xyaxis,EvesEquivocation);
    set(Eve,'EdgeAlpha',0);
    
    %create hatch marks for bob
    for i = 1:2:length(placesBobGetsAllArea)
        firstSpot = placesBobGetsAllArea(i) * scaleBob - closestValue;
        secondSpot = placesBobGetsAllArea(i+1) * scaleBob - closestValue;
        bobPatch1 = patch([firstSpot firstSpot secondSpot secondSpot],...
            [xyaxis(1) xyaxis(end) xyaxis(end) xyaxis(1)],'white');
        
        % Get patch objects from CONTOURGROUP
        bobHandle = findobj(bobPatch1, 'Type', 'patch');
        
        % Apply Hatch Fill
        bobHatch1 = hatchfill(bobHandle, 'cross', 45, 4);
        
        % Remove outline
        set(bobPatch1, 'LineStyle', 'none');
        
        % Change the cross hatch to white
        set(bobHatch1, 'Color', 'red');
        
    end
    
    %set the looks for the graphs
    caxis([0 kBits]);
    colorBar = colorbar;
    colorBar.Label.FontSize = 15;
    colorBar.Label.String = 'Equivocation at Eve (bits/channel use)';
    xlabel('Bobs Position (m)');
    ylabel('Eves Position (m)');
%     title(sprintf('Carriers Below %.0f dB are Bits Coded With RM(%d,5)',10*log10(v2i_snr(k)),u));
    xlim([-200 200]);
    ylim([-200 200]);
    
    %sets the paper layout
    cf = gcf;
    cf.PaperSize = [5 4];
    cf.PaperPosition = [-.05521 0.2240 -0.5521+5 0.2240+4];
    
    hold off;
    drawnow;
end