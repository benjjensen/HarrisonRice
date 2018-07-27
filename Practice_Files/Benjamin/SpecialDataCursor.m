function output_txt = SpecialDataCursor(~,event_obj, vars253)
% event_obj    Handle to event object
% output_txt   Data cursor text string (string or cell array of strings).

   % pos = get(event_obj,'Position');
   pos = event_obj;
    thr = pos(1);
    equ = pos(2);
    dB = pos(3);

    % Goes through for each @dB - calculates and compares - gets u & m   
    counter = 0;  
    for i = 1:55
        centiBels = dB*10;
        obj = sprintf('name = vars%d(%d,1);',centiBels,i);
        eval(obj);

        
        
        searchName = name(11:end);
        [~, lengthName] = size(searchName);
        %%%All on different Plots%%%%
        %    counter2 = 0;
        %%%%
        firstIndex = nan;
        for index = 1 :lengthName
            if searchName(index) == '_'
                u = searchName(1:index-1);
                firstIndex = index+1;
            m = searchName(index+1:end);
            end
        end     
        
        
        k = 0;
        for q = 0 : u
            k = k + nchoosek(m,q);
        end
        n = 2^m;

        rate = k/n;
        carrierRate = max_har(dB-249,1)*rate;       % make sure to put db here

        mu = ceil(ratio(dB-249,1) * n);
        H = weights(1,mu+1);
        percentLeaked = 100*(k-H)/k;
        percentH = 100 - percentLeaked;
        
        if ((carrierRate == thr) && (percentH == equ))
            name = sprintf('RM(%d,%d)',u,m);
        end
        if (counter > 55)
            name = 'error';
        end
    end
    
    
    output_txt = {['X: ',num2str(pos(1),4)],...
    ['Y: ',num2str(pos(2),4)], ...
    ['Code: ' name]};

  
end
