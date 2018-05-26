var Simulation = function () {
    var
    canvas = document.getElementById('canvas'),
    ctx = canvas.getContext('2d'),
    width = canvas.width,
    height = canvas.height,
    interval = 1000,
    num_nodes = 0,
    num_clusters = 0,
    nodes = Array(),
    clusters = Array();
    
    /*
     * Node
     ------------------------------*/
    var Node = function(x, y){
	this.x = Math.random() * width;
	this.y = Math.random() * height;
	this.id = Node.serial++;
    };
    Node.serial = 0;
    Node.prototype.drawLine = function(){
	var c = this.belongTo;
	ctx.beginPath();
	ctx.lineWidth = 2;
	ctx.strokeStyle = c.color;
	ctx.moveTo(c.x, c.y);
	ctx.lineTo(this.x, this.y);
	ctx.stroke();
    };
    Node.prototype.plot = function(){
	ctx.beginPath();
	ctx.lineWidth = 0;
	ctx.arc(this.x, this.y, 3, 0, Math.PI*2, false);
	ctx.fillStyle = 'black';
	ctx.fill();
    };
    Node.prototype.dist = function(c){
	var dx = this.x - c.x, dy = this.y - c.y;
	return (dx*dx + dy*dy);	
    };
    Node.prototype.update = function(){
	for(var i=0; i < num_clusters; i++){
	    var c = clusters[i];
	    if(! this.belongTo){
		c.append(this);
	    }else if(this.dist(c) <= this.dist(this.belongTo) ){
		this.belongTo.remove(this);
		c.append(this);
	    }
	}
    };

    /*
     * Cluster
     ------------------------------*/
    var Cluster = function(){
	this.x = Math.random() * width;
	this.y = Math.random() * height;
	this.nodes = new Array();
	this.color = (function(){
	    var hex = ['3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c'], cl = '#';
	    for(var i = 0; i < 6; i++){
		cl += hex[Math.floor(Math.random() * (hex.length - 1))];
	    }
	    return cl;
	})();
    };
    Cluster.prototype.plot = function(){
	ctx.beginPath();
	ctx.lineWidth = 0;
	ctx.arc(this.x, this.y, 6, 0, Math.PI*2, false);
	ctx.fillStyle = this.color;
	ctx.fill();
    };	
    Cluster.prototype.remove = function(node){
	this.nodes[node.id] = (void 0);
	node.belongTo = null;
    };
    Cluster.prototype.append = function(node){
	this.nodes[node.id] = node;
	node.belongTo = this;
};
    

    /*
     * Methods
     ------------------------------*/
    var initiate = function(){
	num_nodes = document.getElementById('num_nodes').value;
	num_clusters = document.getElementById('num_clusters').value;
	
	for(var i = 0; i < num_clusters; i++){
	    var c = new Cluster();
	    clusters[i] = c;
	    c.plot();
	}

	for(var i = 0; i < num_nodes; i++){
	    var n = new Node();
	    nodes[i] = n;

	    n.plot();
	    n.update();
	    n.drawLine();
	}
    };
    var start = function(){
	setInterval(function(){
	    ctx.fillStyle = "white";
	    ctx.fillRect(0, 0, width, height);

	    for(var i = 0; i < num_clusters; i++){
		var
		num = 0,
		c = clusters[i];

		c.x = 0;
		c.y = 0;

		for(var j = 0; j < num_nodes; j++){
		    if(c.nodes[j] != void 0){
			c.x += c.nodes[j].x;
			c.y += c.nodes[j].y;
			num++;
		    }
		}

		c.x = c.x / num;
		c.y = c.y / num;
		c.plot();
	    }

	    for(var i = 0; i < num_nodes; i++){
		nodes[i].plot();
		nodes[i].update();
		nodes[i].drawLine();
	    }
	}, interval);
    };

    /* canvas initializatoin */
    (function(){
	ctx.fillStyle = "white";
	ctx.fillRect(0, 0, width, height);	
    })();
    
    return {
	start: start,
	initiate: initiate,
    };
} ();
