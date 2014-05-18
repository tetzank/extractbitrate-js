var MP4Check = function(){
	//FIXME: hardcoded size, would be better to load more if needed
	// problem: js file api is async, need synchronous way
	var size = 3*1024*1024;
	
	var $ = this; //copy this pointer to make it usable in callbacks
	$.bitrate = 0;
	$.audioCodec = '';
	$.videoCodec = '';
	
	$.checkFile = function(file, callback){
		var blob = file.slice(0, size);
		var reader = new FileReader();
		
		reader.onloadend = function(evt){
			if(evt.target.readyState == FileReader.DONE){
				var arr = new Uint8Array(evt.target.result);
				var buf = Module._malloc(arr.length);
				Module.HEAPU8.set(arr, buf);

				Module.ccall(
					'process', 'number',
					['number', 'number'],
					[buf, arr.length]);
				
				$.bitrate = Module.ccall(
					'getBitrate', 'number'
				);
				$.audioCodec = Module.ccall(
					'getAudioCodec', 'string'
				);
				$.videoCodec = Module.ccall(
					'getVideoCodec', 'string'
				);
				
				Module._free(buf);
				
				//call callback
				callback.call(null);
			}
		};
		reader.readAsArrayBuffer(blob);
	}
}
