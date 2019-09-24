function displayRawMatfile(filename, N, stride)    
    m = matfile(filename);
    
    details = whos(m, 'data');
    maxIndex = details.size(1);
    
    x = (m.data(1:N,1) - 8192) / 8192;
    
    figure(50);
    plot(x);
    ax = gca;
    ax.YLim = [-1 1];
    
    startIndex = N + 1;
    while startIndex + N < maxIndex
        x = (m.data(startIndex:startIndex+N-1,1) - 8192) / 8192;
        ax.Children.YData = x;
        
        drawnow;
        
        startIndex = startIndex + stride;
    end
end