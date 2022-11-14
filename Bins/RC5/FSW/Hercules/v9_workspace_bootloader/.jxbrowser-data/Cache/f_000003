(function(){/*

 Copyright The Closure Library Authors.
 SPDX-License-Identifier: Apache-2.0
*/
'use strict';var l;function aa(a){var b=0;return function(){return b<a.length?{done:!1,value:a[b++]}:{done:!0}}}
var ba="function"==typeof Object.defineProperties?Object.defineProperty:function(a,b,c){if(a==Array.prototype||a==Object.prototype)return a;a[b]=c.value;return a};
function ca(a){a=["object"==typeof globalThis&&globalThis,a,"object"==typeof window&&window,"object"==typeof self&&self,"object"==typeof global&&global];for(var b=0;b<a.length;++b){var c=a[b];if(c&&c.Math==Math)return c}throw Error("Cannot find global object");}
var ea=ca(this);function n(a,b){if(b)a:{var c=ea;a=a.split(".");for(var d=0;d<a.length-1;d++){var e=a[d];if(!(e in c))break a;c=c[e]}a=a[a.length-1];d=c[a];b=b(d);b!=d&&null!=b&&ba(c,a,{configurable:!0,writable:!0,value:b})}}
n("Symbol",function(a){function b(f){if(this instanceof b)throw new TypeError("Symbol is not a constructor");return new c(d+(f||"")+"_"+e++,f)}
function c(f,g){this.h=f;ba(this,"description",{configurable:!0,writable:!0,value:g})}
if(a)return a;c.prototype.toString=function(){return this.h};
var d="jscomp_symbol_"+(1E9*Math.random()>>>0)+"_",e=0;return b});
n("Symbol.iterator",function(a){if(a)return a;a=Symbol("Symbol.iterator");for(var b="Array Int8Array Uint8Array Uint8ClampedArray Int16Array Uint16Array Int32Array Uint32Array Float32Array Float64Array".split(" "),c=0;c<b.length;c++){var d=ea[b[c]];"function"===typeof d&&"function"!=typeof d.prototype[a]&&ba(d.prototype,a,{configurable:!0,writable:!0,value:function(){return ha(aa(this))}})}return a});
function ha(a){a={next:a};a[Symbol.iterator]=function(){return this};
return a}
function p(a){var b="undefined"!=typeof Symbol&&Symbol.iterator&&a[Symbol.iterator];return b?b.call(a):{next:aa(a)}}
function ia(a){if(!(a instanceof Array)){a=p(a);for(var b,c=[];!(b=a.next()).done;)c.push(b.value);a=c}return a}
function ja(a,b){return Object.prototype.hasOwnProperty.call(a,b)}
var ka="function"==typeof Object.assign?Object.assign:function(a,b){for(var c=1;c<arguments.length;c++){var d=arguments[c];if(d)for(var e in d)ja(d,e)&&(a[e]=d[e])}return a};
n("Object.assign",function(a){return a||ka});
var la="function"==typeof Object.create?Object.create:function(a){function b(){}
b.prototype=a;return new b},na=function(){function a(){function c(){}
new c;Reflect.construct(c,[],function(){});
return new c instanceof c}
if("undefined"!=typeof Reflect&&Reflect.construct){if(a())return Reflect.construct;var b=Reflect.construct;return function(c,d,e){c=b(c,d);e&&Reflect.setPrototypeOf(c,e.prototype);return c}}return function(c,d,e){void 0===e&&(e=c);
e=la(e.prototype||Object.prototype);return Function.prototype.apply.call(c,e,d)||e}}(),pa;
if("function"==typeof Object.setPrototypeOf)pa=Object.setPrototypeOf;else{var ra;a:{var sa={a:!0},ta={};try{ta.__proto__=sa;ra=ta.a;break a}catch(a){}ra=!1}pa=ra?function(a,b){a.__proto__=b;if(a.__proto__!==b)throw new TypeError(a+" is not extensible");return a}:null}var ua=pa;
function u(a,b){a.prototype=la(b.prototype);a.prototype.constructor=a;if(ua)ua(a,b);else for(var c in b)if("prototype"!=c)if(Object.defineProperties){var d=Object.getOwnPropertyDescriptor(b,c);d&&Object.defineProperty(a,c,d)}else a[c]=b[c];a.X=b.prototype}
function va(){this.v=!1;this.l=null;this.i=void 0;this.h=1;this.m=this.o=0;this.s=this.j=null}
function wa(a){if(a.v)throw new TypeError("Generator is already running");a.v=!0}
va.prototype.M=function(a){this.i=a};
function xa(a,b){a.j={Yb:b,fc:!0};a.h=a.o||a.m}
va.prototype.return=function(a){this.j={return:a};this.h=this.m};
function v(a,b,c){a.h=c;return{value:b}}
va.prototype.u=function(a){this.h=a};
function ya(a,b,c){a.o=b;void 0!=c&&(a.m=c)}
function za(a,b){a.h=b;a.o=0}
function Aa(a){a.o=0;var b=a.j.Yb;a.j=null;return b}
function Ba(a){a.s=[a.j];a.o=0;a.m=0}
function Ca(a){var b=a.s.splice(0)[0];(b=a.j=a.j||b)?b.fc?a.h=a.o||a.m:void 0!=b.u&&a.m<b.u?(a.h=b.u,a.j=null):a.h=a.m:a.h=0}
function Da(a){this.h=new va;this.i=a}
function Ea(a,b){wa(a.h);var c=a.h.l;if(c)return Fa(a,"return"in c?c["return"]:function(d){return{value:d,done:!0}},b,a.h.return);
a.h.return(b);return Ga(a)}
function Fa(a,b,c,d){try{var e=b.call(a.h.l,c);if(!(e instanceof Object))throw new TypeError("Iterator result "+e+" is not an object");if(!e.done)return a.h.v=!1,e;var f=e.value}catch(g){return a.h.l=null,xa(a.h,g),Ga(a)}a.h.l=null;d.call(a.h,f);return Ga(a)}
function Ga(a){for(;a.h.h;)try{var b=a.i(a.h);if(b)return a.h.v=!1,{value:b.value,done:!1}}catch(c){a.h.i=void 0,xa(a.h,c)}a.h.v=!1;if(a.h.j){b=a.h.j;a.h.j=null;if(b.fc)throw b.Yb;return{value:b.return,done:!0}}return{value:void 0,done:!0}}
function Ha(a){this.next=function(b){wa(a.h);a.h.l?b=Fa(a,a.h.l.next,b,a.h.M):(a.h.M(b),b=Ga(a));return b};
this.throw=function(b){wa(a.h);a.h.l?b=Fa(a,a.h.l["throw"],b,a.h.M):(xa(a.h,b),b=Ga(a));return b};
this.return=function(b){return Ea(a,b)};
this[Symbol.iterator]=function(){return this}}
function Ia(a){function b(d){return a.next(d)}
function c(d){return a.throw(d)}
return new Promise(function(d,e){function f(g){g.done?d(g.value):Promise.resolve(g.value).then(b,c).then(f,e)}
f(a.next())})}
function x(a){return Ia(new Ha(new Da(a)))}
function Ja(){for(var a=Number(this),b=[],c=a;c<arguments.length;c++)b[c-a]=arguments[c];return b}
n("Reflect",function(a){return a?a:{}});
n("Reflect.construct",function(){return na});
n("Reflect.setPrototypeOf",function(a){return a?a:ua?function(b,c){try{return ua(b,c),!0}catch(d){return!1}}:null});
n("Promise",function(a){function b(g){this.h=0;this.j=void 0;this.i=[];this.v=!1;var h=this.l();try{g(h.resolve,h.reject)}catch(k){h.reject(k)}}
function c(){this.h=null}
function d(g){return g instanceof b?g:new b(function(h){h(g)})}
if(a)return a;c.prototype.i=function(g){if(null==this.h){this.h=[];var h=this;this.j(function(){h.m()})}this.h.push(g)};
var e=ea.setTimeout;c.prototype.j=function(g){e(g,0)};
c.prototype.m=function(){for(;this.h&&this.h.length;){var g=this.h;this.h=[];for(var h=0;h<g.length;++h){var k=g[h];g[h]=null;try{k()}catch(m){this.l(m)}}}this.h=null};
c.prototype.l=function(g){this.j(function(){throw g;})};
b.prototype.l=function(){function g(m){return function(q){k||(k=!0,m.call(h,q))}}
var h=this,k=!1;return{resolve:g(this.K),reject:g(this.m)}};
b.prototype.K=function(g){if(g===this)this.m(new TypeError("A Promise cannot resolve to itself"));else if(g instanceof b)this.P(g);else{a:switch(typeof g){case "object":var h=null!=g;break a;case "function":h=!0;break a;default:h=!1}h?this.I(g):this.o(g)}};
b.prototype.I=function(g){var h=void 0;try{h=g.then}catch(k){this.m(k);return}"function"==typeof h?this.S(h,g):this.o(g)};
b.prototype.m=function(g){this.M(2,g)};
b.prototype.o=function(g){this.M(1,g)};
b.prototype.M=function(g,h){if(0!=this.h)throw Error("Cannot settle("+g+", "+h+"): Promise already settled in state"+this.h);this.h=g;this.j=h;2===this.h&&this.O();this.s()};
b.prototype.O=function(){var g=this;e(function(){if(g.F()){var h=ea.console;"undefined"!==typeof h&&h.error(g.j)}},1)};
b.prototype.F=function(){if(this.v)return!1;var g=ea.CustomEvent,h=ea.Event,k=ea.dispatchEvent;if("undefined"===typeof k)return!0;"function"===typeof g?g=new g("unhandledrejection",{cancelable:!0}):"function"===typeof h?g=new h("unhandledrejection",{cancelable:!0}):(g=ea.document.createEvent("CustomEvent"),g.initCustomEvent("unhandledrejection",!1,!0,g));g.promise=this;g.reason=this.j;return k(g)};
b.prototype.s=function(){if(null!=this.i){for(var g=0;g<this.i.length;++g)f.i(this.i[g]);this.i=null}};
var f=new c;b.prototype.P=function(g){var h=this.l();g.eb(h.resolve,h.reject)};
b.prototype.S=function(g,h){var k=this.l();try{g.call(h,k.resolve,k.reject)}catch(m){k.reject(m)}};
b.prototype.then=function(g,h){function k(w,t){return"function"==typeof w?function(y){try{m(w(y))}catch(E){q(E)}}:t}
var m,q,r=new b(function(w,t){m=w;q=t});
this.eb(k(g,m),k(h,q));return r};
b.prototype.catch=function(g){return this.then(void 0,g)};
b.prototype.eb=function(g,h){function k(){switch(m.h){case 1:g(m.j);break;case 2:h(m.j);break;default:throw Error("Unexpected state: "+m.h);}}
var m=this;null==this.i?f.i(k):this.i.push(k);this.v=!0};
b.resolve=d;b.reject=function(g){return new b(function(h,k){k(g)})};
b.race=function(g){return new b(function(h,k){for(var m=p(g),q=m.next();!q.done;q=m.next())d(q.value).eb(h,k)})};
b.all=function(g){var h=p(g),k=h.next();return k.done?d([]):new b(function(m,q){function r(y){return function(E){w[y]=E;t--;0==t&&m(w)}}
var w=[],t=0;do w.push(void 0),t++,d(k.value).eb(r(w.length-1),q),k=h.next();while(!k.done)})};
return b});
n("WeakMap",function(a){function b(k){this.h=(h+=Math.random()+1).toString();if(k){k=p(k);for(var m;!(m=k.next()).done;)m=m.value,this.set(m[0],m[1])}}
function c(){}
function d(k){var m=typeof k;return"object"===m&&null!==k||"function"===m}
function e(k){if(!ja(k,g)){var m=new c;ba(k,g,{value:m})}}
function f(k){var m=Object[k];m&&(Object[k]=function(q){if(q instanceof c)return q;Object.isExtensible(q)&&e(q);return m(q)})}
if(function(){if(!a||!Object.seal)return!1;try{var k=Object.seal({}),m=Object.seal({}),q=new a([[k,2],[m,3]]);if(2!=q.get(k)||3!=q.get(m))return!1;q.delete(k);q.set(m,4);return!q.has(k)&&4==q.get(m)}catch(r){return!1}}())return a;
var g="$jscomp_hidden_"+Math.random();f("freeze");f("preventExtensions");f("seal");var h=0;b.prototype.set=function(k,m){if(!d(k))throw Error("Invalid WeakMap key");e(k);if(!ja(k,g))throw Error("WeakMap key fail: "+k);k[g][this.h]=m;return this};
b.prototype.get=function(k){return d(k)&&ja(k,g)?k[g][this.h]:void 0};
b.prototype.has=function(k){return d(k)&&ja(k,g)&&ja(k[g],this.h)};
b.prototype.delete=function(k){return d(k)&&ja(k,g)&&ja(k[g],this.h)?delete k[g][this.h]:!1};
return b});
n("Map",function(a){function b(){var h={};return h.previous=h.next=h.head=h}
function c(h,k){var m=h.h;return ha(function(){if(m){for(;m.head!=h.h;)m=m.previous;for(;m.next!=m.head;)return m=m.next,{done:!1,value:k(m)};m=null}return{done:!0,value:void 0}})}
function d(h,k){var m=k&&typeof k;"object"==m||"function"==m?f.has(k)?m=f.get(k):(m=""+ ++g,f.set(k,m)):m="p_"+k;var q=h.data_[m];if(q&&ja(h.data_,m))for(h=0;h<q.length;h++){var r=q[h];if(k!==k&&r.key!==r.key||k===r.key)return{id:m,list:q,index:h,entry:r}}return{id:m,list:q,index:-1,entry:void 0}}
function e(h){this.data_={};this.h=b();this.size=0;if(h){h=p(h);for(var k;!(k=h.next()).done;)k=k.value,this.set(k[0],k[1])}}
if(function(){if(!a||"function"!=typeof a||!a.prototype.entries||"function"!=typeof Object.seal)return!1;try{var h=Object.seal({x:4}),k=new a(p([[h,"s"]]));if("s"!=k.get(h)||1!=k.size||k.get({x:4})||k.set({x:4},"t")!=k||2!=k.size)return!1;var m=k.entries(),q=m.next();if(q.done||q.value[0]!=h||"s"!=q.value[1])return!1;q=m.next();return q.done||4!=q.value[0].x||"t"!=q.value[1]||!m.next().done?!1:!0}catch(r){return!1}}())return a;
var f=new WeakMap;e.prototype.set=function(h,k){h=0===h?0:h;var m=d(this,h);m.list||(m.list=this.data_[m.id]=[]);m.entry?m.entry.value=k:(m.entry={next:this.h,previous:this.h.previous,head:this.h,key:h,value:k},m.list.push(m.entry),this.h.previous.next=m.entry,this.h.previous=m.entry,this.size++);return this};
e.prototype.delete=function(h){h=d(this,h);return h.entry&&h.list?(h.list.splice(h.index,1),h.list.length||delete this.data_[h.id],h.entry.previous.next=h.entry.next,h.entry.next.previous=h.entry.previous,h.entry.head=null,this.size--,!0):!1};
e.prototype.clear=function(){this.data_={};this.h=this.h.previous=b();this.size=0};
e.prototype.has=function(h){return!!d(this,h).entry};
e.prototype.get=function(h){return(h=d(this,h).entry)&&h.value};
e.prototype.entries=function(){return c(this,function(h){return[h.key,h.value]})};
e.prototype.keys=function(){return c(this,function(h){return h.key})};
e.prototype.values=function(){return c(this,function(h){return h.value})};
e.prototype.forEach=function(h,k){for(var m=this.entries(),q;!(q=m.next()).done;)q=q.value,h.call(k,q[1],q[0],this)};
e.prototype[Symbol.iterator]=e.prototype.entries;var g=0;return e});
function Ka(a,b,c){if(null==a)throw new TypeError("The 'this' value for String.prototype."+c+" must not be null or undefined");if(b instanceof RegExp)throw new TypeError("First argument to String.prototype."+c+" must not be a regular expression");return a+""}
n("String.prototype.endsWith",function(a){return a?a:function(b,c){var d=Ka(this,b,"endsWith");b+="";void 0===c&&(c=d.length);c=Math.max(0,Math.min(c|0,d.length));for(var e=b.length;0<e&&0<c;)if(d[--c]!=b[--e])return!1;return 0>=e}});
n("Array.prototype.find",function(a){return a?a:function(b,c){a:{var d=this;d instanceof String&&(d=String(d));for(var e=d.length,f=0;f<e;f++){var g=d[f];if(b.call(c,g,f,d)){b=g;break a}}b=void 0}return b}});
n("String.prototype.startsWith",function(a){return a?a:function(b,c){var d=Ka(this,b,"startsWith");b+="";var e=d.length,f=b.length;c=Math.max(0,Math.min(c|0,d.length));for(var g=0;g<f&&c<e;)if(d[c++]!=b[g++])return!1;return g>=f}});
n("Number.isFinite",function(a){return a?a:function(b){return"number"!==typeof b?!1:!isNaN(b)&&Infinity!==b&&-Infinity!==b}});
n("Number.isInteger",function(a){return a?a:function(b){return Number.isFinite(b)?b===Math.floor(b):!1}});
n("Number.MAX_SAFE_INTEGER",function(){return 9007199254740991});
function La(a,b){a instanceof String&&(a+="");var c=0,d=!1,e={next:function(){if(!d&&c<a.length){var f=c++;return{value:b(f,a[f]),done:!1}}d=!0;return{done:!0,value:void 0}}};
e[Symbol.iterator]=function(){return e};
return e}
n("Array.prototype.entries",function(a){return a?a:function(){return La(this,function(b,c){return[b,c]})}});
n("Array.from",function(a){return a?a:function(b,c,d){c=null!=c?c:function(h){return h};
var e=[],f="undefined"!=typeof Symbol&&Symbol.iterator&&b[Symbol.iterator];if("function"==typeof f){b=f.call(b);for(var g=0;!(f=b.next()).done;)e.push(c.call(d,f.value,g++))}else for(f=b.length,g=0;g<f;g++)e.push(c.call(d,b[g],g));return e}});
n("Array.prototype.keys",function(a){return a?a:function(){return La(this,function(b){return b})}});
n("Array.prototype.values",function(a){return a?a:function(){return La(this,function(b,c){return c})}});
n("Number.isNaN",function(a){return a?a:function(b){return"number"===typeof b&&isNaN(b)}});
n("Object.setPrototypeOf",function(a){return a||ua});
n("Set",function(a){function b(c){this.h=new Map;if(c){c=p(c);for(var d;!(d=c.next()).done;)this.add(d.value)}this.size=this.h.size}
if(function(){if(!a||"function"!=typeof a||!a.prototype.entries||"function"!=typeof Object.seal)return!1;try{var c=Object.seal({x:4}),d=new a(p([c]));if(!d.has(c)||1!=d.size||d.add(c)!=d||1!=d.size||d.add({x:4})!=d||2!=d.size)return!1;var e=d.entries(),f=e.next();if(f.done||f.value[0]!=c||f.value[1]!=c)return!1;f=e.next();return f.done||f.value[0]==c||4!=f.value[0].x||f.value[1]!=f.value[0]?!1:e.next().done}catch(g){return!1}}())return a;
b.prototype.add=function(c){c=0===c?0:c;this.h.set(c,c);this.size=this.h.size;return this};
b.prototype.delete=function(c){c=this.h.delete(c);this.size=this.h.size;return c};
b.prototype.clear=function(){this.h.clear();this.size=0};
b.prototype.has=function(c){return this.h.has(c)};
b.prototype.entries=function(){return this.h.entries()};
b.prototype.values=function(){return this.h.values()};
b.prototype.keys=b.prototype.values;b.prototype[Symbol.iterator]=b.prototype.values;b.prototype.forEach=function(c,d){var e=this;this.h.forEach(function(f){return c.call(d,f,f,e)})};
return b});
n("Object.entries",function(a){return a?a:function(b){var c=[],d;for(d in b)ja(b,d)&&c.push([d,b[d]]);return c}});
n("Object.is",function(a){return a?a:function(b,c){return b===c?0!==b||1/b===1/c:b!==b&&c!==c}});
n("Array.prototype.includes",function(a){return a?a:function(b,c){var d=this;d instanceof String&&(d=String(d));var e=d.length;c=c||0;for(0>c&&(c=Math.max(c+e,0));c<e;c++){var f=d[c];if(f===b||Object.is(f,b))return!0}return!1}});
n("String.prototype.includes",function(a){return a?a:function(b,c){return-1!==Ka(this,b,"includes").indexOf(b,c||0)}});
n("globalThis",function(a){return a||ea});
n("Object.values",function(a){return a?a:function(b){var c=[],d;for(d in b)ja(b,d)&&c.push(b[d]);return c}});
var Ma=Ma||{},z=this||self;function A(a,b,c){a=a.split(".");c=c||z;a[0]in c||"undefined"==typeof c.execScript||c.execScript("var "+a[0]);for(var d;a.length&&(d=a.shift());)a.length||void 0===b?c[d]&&c[d]!==Object.prototype[d]?c=c[d]:c=c[d]={}:c[d]=b}
function B(a,b){a=a.split(".");b=b||z;for(var c=0;c<a.length;c++)if(b=b[a[c]],null==b)return null;return b}
function Na(a){a.zb=void 0;a.getInstance=function(){return a.zb?a.zb:a.zb=new a}}
function Oa(a){var b=typeof a;return"object"!=b?b:a?Array.isArray(a)?"array":b:"null"}
function Pa(a){var b=Oa(a);return"array"==b||"object"==b&&"number"==typeof a.length}
function Qa(a){var b=typeof a;return"object"==b&&null!=a||"function"==b}
function Ra(a){return Object.prototype.hasOwnProperty.call(a,Sa)&&a[Sa]||(a[Sa]=++Ta)}
var Sa="closure_uid_"+(1E9*Math.random()>>>0),Ta=0;function Ua(a,b,c){return a.call.apply(a.bind,arguments)}
function Va(a,b,c){if(!a)throw Error();if(2<arguments.length){var d=Array.prototype.slice.call(arguments,2);return function(){var e=Array.prototype.slice.call(arguments);Array.prototype.unshift.apply(e,d);return a.apply(b,e)}}return function(){return a.apply(b,arguments)}}
function Xa(a,b,c){Function.prototype.bind&&-1!=Function.prototype.bind.toString().indexOf("native code")?Xa=Ua:Xa=Va;return Xa.apply(null,arguments)}
function Ya(a,b){var c=Array.prototype.slice.call(arguments,1);return function(){var d=c.slice();d.push.apply(d,arguments);return a.apply(this,d)}}
function Za(a,b){function c(){}
c.prototype=b.prototype;a.X=b.prototype;a.prototype=new c;a.prototype.constructor=a;a.jr=function(d,e,f){for(var g=Array(arguments.length-2),h=2;h<arguments.length;h++)g[h-2]=arguments[h];return b.prototype[e].apply(d,g)}}
function $a(a){return a}
;function ab(a,b){if(Error.captureStackTrace)Error.captureStackTrace(this,ab);else{var c=Error().stack;c&&(this.stack=c)}a&&(this.message=String(a));void 0!==b&&(this.cause=b)}
Za(ab,Error);ab.prototype.name="CustomError";function bb(a){a=a.url;var b=/[?&]dsh=1(&|$)/.test(a);this.j=!b&&/[?&]ae=1(&|$)/.test(a);this.l=!b&&/[?&]ae=2(&|$)/.test(a);if((this.h=/[?&]adurl=([^&]*)/.exec(a))&&this.h[1]){try{var c=decodeURIComponent(this.h[1])}catch(d){c=null}this.i=c}}
;function cb(){}
function db(a){var b=!1,c;return function(){b||(c=a(),b=!0);return c}}
;var eb=Array.prototype.indexOf?function(a,b){return Array.prototype.indexOf.call(a,b,void 0)}:function(a,b){if("string"===typeof a)return"string"!==typeof b||1!=b.length?-1:a.indexOf(b,0);
for(var c=0;c<a.length;c++)if(c in a&&a[c]===b)return c;return-1},fb=Array.prototype.forEach?function(a,b,c){Array.prototype.forEach.call(a,b,c)}:function(a,b,c){for(var d=a.length,e="string"===typeof a?a.split(""):a,f=0;f<d;f++)f in e&&b.call(c,e[f],f,a)},gb=Array.prototype.filter?function(a,b){return Array.prototype.filter.call(a,b,void 0)}:function(a,b){for(var c=a.length,d=[],e=0,f="string"===typeof a?a.split(""):a,g=0;g<c;g++)if(g in f){var h=f[g];
b.call(void 0,h,g,a)&&(d[e++]=h)}return d},hb=Array.prototype.map?function(a,b){return Array.prototype.map.call(a,b,void 0)}:function(a,b){for(var c=a.length,d=Array(c),e="string"===typeof a?a.split(""):a,f=0;f<c;f++)f in e&&(d[f]=b.call(void 0,e[f],f,a));
return d},ib=Array.prototype.reduce?function(a,b,c){return Array.prototype.reduce.call(a,b,c)}:function(a,b,c){var d=c;
fb(a,function(e,f){d=b.call(void 0,d,e,f,a)});
return d};
function jb(a,b){a:{for(var c=a.length,d="string"===typeof a?a.split(""):a,e=0;e<c;e++)if(e in d&&b.call(void 0,d[e],e,a)){b=e;break a}b=-1}return 0>b?null:"string"===typeof a?a.charAt(b):a[b]}
function kb(a,b){b=eb(a,b);var c;(c=0<=b)&&Array.prototype.splice.call(a,b,1);return c}
function lb(a,b){for(var c=1;c<arguments.length;c++){var d=arguments[c];if(Pa(d)){var e=a.length||0,f=d.length||0;a.length=e+f;for(var g=0;g<f;g++)a[e+g]=d[g]}else a.push(d)}}
;function mb(a,b){for(var c in a)b.call(void 0,a[c],c,a)}
function nb(a){var b=ob,c;for(c in b)if(a.call(void 0,b[c],c,b))return c}
function pb(a){for(var b in a)return!1;return!0}
function rb(a,b){if(null!==a&&b in a)throw Error('The object already contains the key "'+b+'"');a[b]=!0}
function sb(a){return null!==a&&"privembed"in a?a.privembed:!1}
function tb(a,b){for(var c in a)if(!(c in b)||a[c]!==b[c])return!1;for(var d in b)if(!(d in a))return!1;return!0}
function ub(a){var b={},c;for(c in a)b[c]=a[c];return b}
function vb(a){if(!a||"object"!==typeof a)return a;if("function"===typeof a.clone)return a.clone();if("undefined"!==typeof Map&&a instanceof Map)return new Map(a);if("undefined"!==typeof Set&&a instanceof Set)return new Set(a);if(a instanceof Date)return new Date(a.getTime());var b=Array.isArray(a)?[]:"function"!==typeof ArrayBuffer||"function"!==typeof ArrayBuffer.isView||!ArrayBuffer.isView(a)||a instanceof DataView?{}:new a.constructor(a.length),c;for(c in a)b[c]=vb(a[c]);return b}
var wb="constructor hasOwnProperty isPrototypeOf propertyIsEnumerable toLocaleString toString valueOf".split(" ");function xb(a,b){for(var c,d,e=1;e<arguments.length;e++){d=arguments[e];for(c in d)a[c]=d[c];for(var f=0;f<wb.length;f++)c=wb[f],Object.prototype.hasOwnProperty.call(d,c)&&(a[c]=d[c])}}
;var yb;function zb(){if(void 0===yb){var a=null,b=z.trustedTypes;if(b&&b.createPolicy){try{a=b.createPolicy("goog#html",{createHTML:$a,createScript:$a,createScriptURL:$a})}catch(c){z.console&&z.console.error(c.message)}yb=a}else yb=a}return yb}
;function Ab(a,b){this.j=a===Bb&&b||""}
Ab.prototype.i=!0;Ab.prototype.h=function(){return this.j};
function Cb(a){return new Ab(Bb,a)}
var Bb={};Cb("");var Db={};function Eb(a){this.j=Db===Db?a:"";this.i=!0}
Eb.prototype.toString=function(){return this.j.toString()};
Eb.prototype.h=function(){return this.j.toString()};function Fb(a,b){this.j=b===Gb?a:""}
Fb.prototype.toString=function(){return this.j+""};
Fb.prototype.i=!0;Fb.prototype.h=function(){return this.j.toString()};
function Hb(a){if(a instanceof Fb&&a.constructor===Fb)return a.j;Oa(a);return"type_error:TrustedResourceUrl"}
var Gb={};function Ib(a){var b=zb();a=b?b.createScriptURL(a):a;return new Fb(a,Gb)}
;var Lb=String.prototype.trim?function(a){return a.trim()}:function(a){return/^[\s\xa0]*([\s\S]*?)[\s\xa0]*$/.exec(a)[1]};
function Mb(a,b){return a<b?-1:a>b?1:0}
;function Nb(a,b){this.j=b===Ob?a:""}
Nb.prototype.toString=function(){return this.j.toString()};
Nb.prototype.i=!0;Nb.prototype.h=function(){return this.j.toString()};
function Pb(a){if(a instanceof Nb&&a.constructor===Nb)return a.j;Oa(a);return"type_error:SafeUrl"}
var Qb=/^(?:(?:https?|mailto|ftp):|[^:/?#]*(?:[/?#]|$))/i,Ob={},Rb=new Nb("about:invalid#zClosurez",Ob);function Sb(){var a=z.navigator;return a&&(a=a.userAgent)?a:""}
function C(a){return-1!=Sb().indexOf(a)}
;function Tb(){return C("Trident")||C("MSIE")}
function Ub(){return C("Firefox")||C("FxiOS")}
function Vb(){return C("Safari")&&!(Wb()||C("Coast")||C("Opera")||C("Edge")||C("Edg/")||C("OPR")||Ub()||C("Silk")||C("Android"))}
function Wb(){return(C("Chrome")||C("CriOS"))&&!C("Edge")||C("Silk")}
function Xb(){return C("Android")&&!(Wb()||Ub()||C("Opera")||C("Silk"))}
function Yb(a){var b={};a.forEach(function(c){b[c[0]]=c[1]});
return function(c){return b[c.find(function(d){return d in b})]||""}}
function Zb(a){var b=Sb();if("Internet Explorer"===a){if(Tb())if((a=/rv: *([\d\.]*)/.exec(b))&&a[1])b=a[1];else{a="";var c=/MSIE +([\d\.]+)/.exec(b);if(c&&c[1])if(b=/Trident\/(\d.\d)/.exec(b),"7.0"==c[1])if(b&&b[1])switch(b[1]){case "4.0":a="8.0";break;case "5.0":a="9.0";break;case "6.0":a="10.0";break;case "7.0":a="11.0"}else a="7.0";else a=c[1];b=a}else b="";return b}var d=RegExp("([A-Z][\\w ]+)/([^\\s]+)\\s*(?:\\((.*?)\\))?","g");c=[];for(var e;e=d.exec(b);)c.push([e[1],e[2],e[3]||void 0]);b=Yb(c);
switch(a){case "Opera":if(C("Opera"))return b(["Version","Opera"]);if(C("OPR"))return b(["OPR"]);break;case "Microsoft Edge":if(C("Edge"))return b(["Edge"]);if(C("Edg/"))return b(["Edg"]);break;case "Chromium":if(Wb())return b(["Chrome","CriOS","HeadlessChrome"])}return"Firefox"===a&&Ub()||"Safari"===a&&Vb()||"Android Browser"===a&&Xb()||"Silk"===a&&C("Silk")?(b=c[2])&&b[1]||"":""}
function bc(a){a=Zb(a);if(""===a)return NaN;a=a.split(".");return 0===a.length?NaN:Number(a[0])}
;var cc={};function dc(a){this.j=cc===cc?a:"";this.i=!0}
dc.prototype.h=function(){return this.j.toString()};
dc.prototype.toString=function(){return this.j.toString()};function ec(a,b){b instanceof Nb||b instanceof Nb||(b="object"==typeof b&&b.i?b.h():String(b),Qb.test(b)||(b="about:invalid#zClosurez"),b=new Nb(b,Ob));a.href=Pb(b)}
function fc(a,b){a.rel="stylesheet";a.href=Hb(b).toString();(b=gc('style[nonce],link[rel="stylesheet"][nonce]',a.ownerDocument&&a.ownerDocument.defaultView))&&a.setAttribute("nonce",b)}
function hc(){return gc("script[nonce]")}
var ic=/^[\w+/_-]+[=]{0,2}$/;function gc(a,b){b=(b||z).document;return b.querySelector?(a=b.querySelector(a))&&(a=a.nonce||a.getAttribute("nonce"))&&ic.test(a)?a:"":""}
;function jc(a){for(var b=0,c=0;c<a.length;++c)b=31*b+a.charCodeAt(c)>>>0;return b}
;var kc=RegExp("^(?:([^:/?#.]+):)?(?://(?:([^\\\\/?#]*)@)?([^\\\\/?#]*?)(?::([0-9]+))?(?=[\\\\/?#]|$))?([^?#]+)?(?:\\?([^#]*))?(?:#([\\s\\S]*))?$");function lc(a){return a?decodeURI(a):a}
function mc(a,b){return b.match(kc)[a]||null}
function nc(a){return lc(mc(3,a))}
function oc(a){var b=a.match(kc);a=b[5];var c=b[6];b=b[7];var d="";a&&(d+=a);c&&(d+="?"+c);b&&(d+="#"+b);return d}
function pc(a,b){if(!b)return a;var c=a.indexOf("#");0>c&&(c=a.length);var d=a.indexOf("?");if(0>d||d>c){d=c;var e=""}else e=a.substring(d+1,c);a=[a.slice(0,d),e,a.slice(c)];c=a[1];a[1]=b?c?c+"&"+b:b:c;return a[0]+(a[1]?"?"+a[1]:"")+a[2]}
function qc(a,b,c){if(Array.isArray(b))for(var d=0;d<b.length;d++)qc(a,String(b[d]),c);else null!=b&&c.push(a+(""===b?"":"="+encodeURIComponent(String(b))))}
function rc(a,b){var c=[];for(b=b||0;b<a.length;b+=2)qc(a[b],a[b+1],c);return c.join("&")}
function sc(a){var b=[],c;for(c in a)qc(c,a[c],b);return b.join("&")}
function tc(a,b){var c=2==arguments.length?rc(arguments[1],0):rc(arguments,1);return pc(a,c)}
function uc(a,b){b=sc(b);return pc(a,b)}
function vc(a,b,c){c=null!=c?"="+encodeURIComponent(String(c)):"";return pc(a,b+c)}
function wc(a,b,c,d){for(var e=c.length;0<=(b=a.indexOf(c,b))&&b<d;){var f=a.charCodeAt(b-1);if(38==f||63==f)if(f=a.charCodeAt(b+e),!f||61==f||38==f||35==f)return b;b+=e+1}return-1}
var xc=/#|$/,yc=/[?&]($|#)/;function Cc(a,b){for(var c=a.search(xc),d=0,e,f=[];0<=(e=wc(a,d,b,c));)f.push(a.substring(d,e)),d=Math.min(a.indexOf("&",e)+1||c,c);f.push(a.slice(d));return f.join("").replace(yc,"$1")}
;function Dc(a){z.setTimeout(function(){throw a;},0)}
;function Ec(){return C("iPhone")&&!C("iPod")&&!C("iPad")}
function Fc(){var a=Sb(),b="";C("Windows")?(b=/Windows (?:NT|Phone) ([0-9.]+)/,b=(a=b.exec(a))?a[1]:"0.0"):Ec()||C("iPad")||C("iPod")?(b=/(?:iPhone|iPod|iPad|CPU)\s+OS\s+(\S+)/,b=(a=b.exec(a))&&a[1].replace(/_/g,".")):C("Macintosh")?(b=/Mac OS X ([0-9_.]+)/,b=(a=b.exec(a))?a[1].replace(/_/g,"."):"10"):-1!=Sb().toLowerCase().indexOf("kaios")?(b=/(?:KaiOS)\/(\S+)/i,b=(a=b.exec(a))&&a[1]):C("Android")?(b=/Android\s+([^\);]+)(\)|;)/,b=(a=b.exec(a))&&a[1]):C("CrOS")&&(b=/(?:CrOS\s+(?:i686|x86_64)\s+([0-9.]+))/,
b=(a=b.exec(a))&&a[1]);a=0;b=Lb(String(b||"")).split(".");for(var c=Lb("12").split("."),d=Math.max(b.length,c.length),e=0;0==a&&e<d;e++){var f=b[e]||"",g=c[e]||"";do{f=/(\d*)(\D*)(.*)/.exec(f)||["","","",""];g=/(\d*)(\D*)(.*)/.exec(g)||["","","",""];if(0==f[0].length&&0==g[0].length)break;a=Mb(0==f[1].length?0:parseInt(f[1],10),0==g[1].length?0:parseInt(g[1],10))||Mb(0==f[2].length,0==g[2].length)||Mb(f[2],g[2]);f=f[3];g=g[3]}while(0==a)}}
;function Gc(a){Gc[" "](a);return a}
Gc[" "]=function(){};var Hc=C("Opera"),Ic=Tb(),Jc=C("Edge"),Kc=C("Gecko")&&!(-1!=Sb().toLowerCase().indexOf("webkit")&&!C("Edge"))&&!(C("Trident")||C("MSIE"))&&!C("Edge"),Lc=-1!=Sb().toLowerCase().indexOf("webkit")&&!C("Edge"),Mc=C("Android");function Nc(){var a=z.document;return a?a.documentMode:void 0}
var Oc;a:{var Pc="",Qc=function(){var a=Sb();if(Kc)return/rv:([^\);]+)(\)|;)/.exec(a);if(Jc)return/Edge\/([\d\.]+)/.exec(a);if(Ic)return/\b(?:MSIE|rv)[: ]([^\);]+)(\)|;)/.exec(a);if(Lc)return/WebKit\/(\S+)/.exec(a);if(Hc)return/(?:Version)[ \/]?(\S+)/.exec(a)}();
Qc&&(Pc=Qc?Qc[1]:"");if(Ic){var Rc=Nc();if(null!=Rc&&Rc>parseFloat(Pc)){Oc=String(Rc);break a}}Oc=Pc}var Sc=Oc,Tc;if(z.document&&Ic){var Uc=Nc();Tc=Uc?Uc:parseInt(Sc,10)||void 0}else Tc=void 0;var Vc=Tc;var Wc=Ec()||C("iPod"),Xc=C("iPad");Xb();Wb();var Yc=Vb()&&!(Ec()||C("iPad")||C("iPod"));var Zc={},$c=null;function ad(a,b){Pa(a);void 0===b&&(b=0);bd();b=Zc[b];for(var c=Array(Math.floor(a.length/3)),d=b[64]||"",e=0,f=0;e<a.length-2;e+=3){var g=a[e],h=a[e+1],k=a[e+2],m=b[g>>2];g=b[(g&3)<<4|h>>4];h=b[(h&15)<<2|k>>6];k=b[k&63];c[f++]=""+m+g+h+k}m=0;k=d;switch(a.length-e){case 2:m=a[e+1],k=b[(m&15)<<2]||d;case 1:a=a[e],c[f]=""+b[a>>2]+b[(a&3)<<4|m>>4]+k+d}return c.join("")}
function cd(a){var b=a.length,c=3*b/4;c%3?c=Math.floor(c):-1!="=.".indexOf(a[b-1])&&(c=-1!="=.".indexOf(a[b-2])?c-2:c-1);var d=new Uint8Array(c),e=0;dd(a,function(f){d[e++]=f});
return e!==c?d.subarray(0,e):d}
function dd(a,b){function c(k){for(;d<a.length;){var m=a.charAt(d++),q=$c[m];if(null!=q)return q;if(!/^[\s\xa0]*$/.test(m))throw Error("Unknown base64 encoding at char: "+m);}return k}
bd();for(var d=0;;){var e=c(-1),f=c(0),g=c(64),h=c(64);if(64===h&&-1===e)break;b(e<<2|f>>4);64!=g&&(b(f<<4&240|g>>2),64!=h&&b(g<<6&192|h))}}
function bd(){if(!$c){$c={};for(var a="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789".split(""),b=["+/=","+/","-_=","-_.","-_"],c=0;5>c;c++){var d=a.concat(b[c].split(""));Zc[c]=d;for(var e=0;e<d.length;e++){var f=d[e];void 0===$c[f]&&($c[f]=e)}}}}
;var ed="undefined"!==typeof Uint8Array;function fd(a){return ed&&null!=a&&a instanceof Uint8Array}
var gd={};var hd;function id(a){if(a!==gd)throw Error("illegal external caller");}
function jd(a,b){id(b);this.ba=a;if(null!=a&&0===a.length)throw Error("ByteString should be constructed with non-empty values");}
function kd(){return hd||(hd=new jd(null,gd))}
jd.prototype.Oa=function(){return null==this.ba};
jd.prototype.sizeBytes=function(){id(gd);var a=this.ba;null==a||fd(a)||("string"===typeof a?a=cd(a):(Oa(a),a=null));return(a=null==a?a:this.ba=a)?a.length:0};var ld="function"===typeof Symbol&&"symbol"===typeof Symbol()?Symbol():void 0;function md(a,b){ld?a[ld]|=b:void 0!==a.fa?a.fa|=b:Object.defineProperties(a,{fa:{value:b,configurable:!0,writable:!0,enumerable:!1}})}
function nd(a,b){ld?a[ld]&&(a[ld]&=~b):void 0!==a.fa&&(a.fa&=~b)}
function od(a){var b;ld?b=a[ld]:b=a.fa;return null==b?0:b}
function pd(a,b){ld?a[ld]=b:void 0!==a.fa?a.fa=b:Object.defineProperties(a,{fa:{value:b,configurable:!0,writable:!0,enumerable:!1}})}
function qd(a){md(a,1);return a}
function vd(a){return!!(od(a)&2)}
function wd(a){md(a,16);return a}
function xd(a){md(a,48);return a}
function yd(a){var b=void 0===b?!0:b;var c=od(a);pd(a,b?c|32:c&-33);return!!(c&32)}
function zd(a,b){pd(b,(od(a)|0)&-51)}
function Ad(a,b){pd(b,(od(a)|18)&-41)}
function Bd(a,b){var c=od(a);(c&b)!==b&&(Object.isFrozen(a)&&(a=Array.prototype.slice.call(a)),pd(a,c|b));return a}
;var Cd={};function Dd(a){return null!==a&&"object"===typeof a&&!Array.isArray(a)&&a.constructor===Object}
var Ed,Fd,Gd=[];pd(Gd,23);Fd=Object.freeze(Gd);function Hd(a){if(vd(a.B))throw Error("Cannot mutate an immutable Message");}
function Id(a){var b=a.length;(b=b?a[b-1]:void 0)&&Dd(b)?b.g=1:(b={},a.push((b.g=1,b)))}
;function Jd(a){return a.displayName||a.name||"unknown type name"}
function Kd(a,b){if(!(a instanceof b))throw Error("Expected instanceof "+Jd(b)+" but got "+(a&&Jd(a.constructor)));return a}
;var Ld;function Md(a){switch(typeof a){case "number":return isFinite(a)?a:String(a);case "object":if(a)if(Array.isArray(a)){if(0!==(od(a)&128))return a=Array.prototype.slice.call(a),Id(a),a}else{if(fd(a))return ad(a);if(a instanceof jd){var b=a.ba;return null==b?"":"string"===typeof b?b:a.ba=ad(b)}}}return a}
;function Nd(a,b,c,d){if(null!=a){if(Array.isArray(a))a=Od(a,b,c,void 0!==d);else if(Dd(a)){var e={},f;for(f in a)e[f]=Nd(a[f],b,c,d);a=e}else a=b(a,d);return a}}
function Od(a,b,c,d){d=d?!!(od(a)&16):void 0;for(var e=Array.prototype.slice.call(a),f=0;f<e.length;f++)e[f]=Nd(e[f],b,c,d);c(a,e);return e}
function Pd(a){return a.jb===Cd?a.toJSON():Md(a)}
function Qd(a){if(!a)return a;if("object"===typeof a){if(fd(a))return new Uint8Array(a);if(a.jb===Cd)return a.clone()}return a}
function Rd(a,b){0!==(od(a)&128)&&Id(b)}
;function Sd(a){return a.j||(a.j=a.B[a.l+a.ta]={})}
function Td(a,b,c){return-1===b?null:b>=a.l?a.j?a.j[b]:void 0:c&&a.j&&(c=a.j[b],null!=c)?c:a.B[b+a.ta]}
function D(a,b,c,d){Hd(a);return Ud(a,b,c,d)}
function Ud(a,b,c,d){a.o&&(a.o=void 0);if(b>=a.l||d)return Sd(a)[b]=c,a;a.B[b+a.ta]=c;(c=a.j)&&b in c&&delete c[b];return a}
function Vd(a,b){a&&vd(b.B)&&vd(a.B);return a}
function Wd(a,b,c,d,e){var f=Td(a,b,d);Array.isArray(f)||(f=Fd);var g=od(f);g&1||qd(f);if(e)g&2||md(f,2),c&1||Object.freeze(f);else{e=!(c&2);var h=g&2;c&1||!h?e&&g&16&&!h&&nd(f,16):(f=qd(Array.prototype.slice.call(f)),Ud(a,b,f,d))}return f}
function Xd(a,b,c,d){Hd(a);(c=Yd(a,c))&&c!==b&&null!=d&&Ud(a,c,void 0,!1);return Ud(a,b,d)}
function Yd(a,b){for(var c=0,d=0;d<b.length;d++){var e=b[d];null!=Td(a,e)&&(0!==c&&Ud(a,c,void 0,!1),c=e)}return c}
function Zd(a,b,c,d){var e=d=void 0===d?!1:d,f=Td(a,c,e);var g=!1;var h=null==f||"object"!==typeof f||(g=Array.isArray(f))||f.jb!==Cd?g?new b(f):void 0:f;h!==f&&null!=h&&(Ud(a,c,h,e),md(h.B,od(a.B)&-33));b=Vd(h,a);if(null==b)return b;vd(a.B)||(e=$d(b),e!==b&&(b=e,Ud(a,c,b,d)));return Vd(b,a)}
function ae(a,b,c,d,e,f){a.h||(a.h={});var g=a.h[c],h=Wd(a,c,3,d,f);if(g)f||(Object.isFrozen(g)?e||(g=Array.prototype.slice.call(g),a.h[c]=g):e&&Object.freeze(g));else{g=[];var k=!!(od(a.B)&16),m=vd(h);!f&&m&&(h=qd(Array.prototype.slice.call(h)),Ud(a,c,h,d));d=m;for(var q=0;q<h.length;q++){var r=h[q];var w=b;var t=k,y=!1;y=void 0===y?!1:y;t=void 0===t?!1:t;w=Array.isArray(r)?new w(t?wd(r):r):y?new w:void 0;void 0!==w&&(d=d||vd(r),g.push(w),m&&md(w.B,2))}a.h[c]=g;a=h;b=!d;Object.isFrozen(a)||(c=od(a)|
33,pd(a,b?c|8:c&-9));(f||e&&m)&&md(g,2);(f||e)&&Object.freeze(g)}return g}
function be(a,b,c,d){var e=vd(a.B);b=ae(a,b,c,d,e,e);a=Wd(a,c,3,d,e);if(!e&&a&&!(od(a)&8)){for(e=0;e<b.length;e++)c=b[e],d=$d(c),c!==d&&(b[e]=d,a[e]=b[e].B);md(a,8)}return b}
function G(a,b,c,d){Hd(a);null!=d?Kd(d,b):d=void 0;return Ud(a,c,d)}
function ce(a,b,c,d,e){Hd(a);null!=e?Kd(e,b):e=void 0;Xd(a,c,d,e)}
function de(a,b,c,d,e){Hd(a);if(null!=d){var f=qd([]);for(var g=!1,h=0;h<d.length;h++)f[h]=Kd(d[h],b).B,g=g||vd(f[h]);a.h||(a.h={});a.h[c]=d;b=f;g?nd(b,8):md(b,8)}else a.h&&(a.h[c]=void 0),f=Fd;return Ud(a,c,f,e)}
function ee(a,b,c,d){Hd(a);var e=ae(a,c,b,void 0,!1,!1);c=null!=d?Kd(d,c):new c;a=Wd(a,b,2,void 0,!1);e.push(c);a.push(c.B);vd(c.B)&&nd(a,8)}
function fe(a,b){return Td(a,b)}
function ge(a,b){return null==a?b:a}
;function he(a,b){if(null!=a){if(ed&&a instanceof Uint8Array)return a.length?new jd(new Uint8Array(a),gd):kd();if(Array.isArray(a)){if(vd(a))return a;b&&(b=od(a),b=!(b&32)&&(!!(b&16)||0===b));if(b)return md(a,2),a;a=Od(a,he,Ad);od(a)&4&&Object.freeze(a);return a}return a.jb===Cd?ie(a):a}}
function je(a,b,c,d,e,f){(a=a.h&&a.h[c])?(d=0<a.length?a[0].constructor:void 0,vd(a)&&Object.isFrozen(a)||(f=hb(a,ie),Ad(a,f),Object.freeze(f),a=f),de(b,d,c,a,e)):D(b,c,he(d,f),e)}
function ie(a){if(vd(a.B))return a;a=ke(a);md(a.B,2);return a}
function ke(a){var b=a.B,c=wd([]),d=a.constructor.h;d&&c.push(d);0!==(od(b)&128)&&Id(c);d=a.constructor;od(c);Ld=c;c=new d(c);Ld=void 0;a.Na&&(c.Na=a.Na.slice());d=!!(od(b)&16);for(var e=0;e<b.length;e++){var f=b[e];if(e===b.length-1&&Dd(f))for(var g in f){var h=+g;Number.isNaN(h)?Sd(c)[h]=f[h]:je(a,c,h,f[g],!0,d)}else je(a,c,e-a.ta,f,!1,d)}return c}
function $d(a){if(!vd(a.B))return a;var b=ke(a);b.o=a;return b}
;function H(a,b,c){null==a&&(a=Ld);Ld=void 0;var d=this.constructor.i||0,e=0<d,f=this.constructor.h,g=!1,h=!1;if(null==a)a=xd(f?[f]:[]),g=!0;else{if(!Array.isArray(a))throw Error();if(f&&f!==a[0])throw Error();if(g=!!(od(a)&16))h=yd(a);if(e)if(0!==(od(a)&128))d=0;else if(0<a.length){var k=a[a.length-1];if(Dd(k)&&"g"in k){d=0;md(a,128);delete k.g;var m=!0,q;for(q in k){m=!1;break}m&&a.pop()}}}this.ta=(f?0:-1)-d;this.h=void 0;this.B=a;le(this,b);if(!e){if(this.j&&"g"in this.j)throw Error('Unexpected "g" flag in sparse object of message that is not a group type.');
if(0!==(od(a)&128))throw Error();}if(c){var r=1|(g&&!h?16:0);a=function(w){if(Array.isArray(w)){var t=od(w);(t&r)===r||Object.isFrozen(w)||pd(w,t|r)}};
for(b=0;b<c.length;b++)d=c[b],d<this.l?(d+=this.ta,(e=this.B[d])?a(e):this.B[d]=Fd):(e=Sd(this),(f=e[d])?a(f):e[d]=Fd)}}
H.prototype.toJSON=function(){var a=this.B,b;Ed?b=a:b=Od(a,Pd,Rd);return b};
function me(a){Ed=!0;try{return JSON.stringify(a.toJSON(),ne)}finally{Ed=!1}}
H.prototype.clone=function(){var a=Od(this.B,Qd,zd);wd(a);Ld=a;a=new this.constructor(a);Ld=void 0;oe(a,this);return a};
function le(a,b){var c=a.B.length,d=c-1;if(c&&(c=a.B[d],Dd(c))){a.j=c;a.l=d-a.ta;return}void 0!==b&&-1<b?(a.l=Math.max(b,d+1-a.ta),a.j=void 0):a.l=Number.MAX_VALUE}
H.prototype.jb=Cd;H.prototype.toString=function(){return this.B.toString()};
function ne(a,b){return Md(b)}
function oe(a,b){b.Na&&(a.Na=b.Na.slice());var c=b.h;if(c){b=b.j;for(var d in c){var e=c[d];if(e){var f=!(!b||!b[d]),g=+d;if(Array.isArray(e)){if(e.length)for(f=be(a,e[0].constructor,g,f),g=0;g<Math.min(f.length,e.length);g++)oe(f[g],e[g])}else throw Error("unexpected object: type: "+Oa(e)+": "+e);}}}}
;function pe(a){var b=this.h,c=this.i;return this.isRepeated?be(a,b,c,!0):Zd(a,b,c,!0)}
;function qe(a){this.Sb=a}
;function re(a,b,c){this.i=a;this.l=b;this.h=c||[];this.Aa=new Map}
l=re.prototype;l.Fc=function(a){var b=Ja.apply(1,arguments),c=this.ub(b);c?c.push(new qe(a)):this.sc(a,b)};
l.sc=function(a){this.Aa.set(this.Zb(Ja.apply(1,arguments)),[new qe(a)])};
l.ub=function(){var a=this.Zb(Ja.apply(0,arguments));return this.Aa.has(a)?this.Aa.get(a):void 0};
l.Rc=function(){var a=this.ub(Ja.apply(0,arguments));return a&&a.length?a[0]:void 0};
l.clear=function(){this.Aa.clear()};
l.Zb=function(){var a=Ja.apply(0,arguments);return a?a.join(","):"key"};function se(a,b){re.call(this,a,3,b)}
u(se,re);se.prototype.j=function(a){var b=Ja.apply(1,arguments),c=0,d=this.Rc(b);d&&(c=d.Sb);this.sc(c+a,b)};function te(a,b){re.call(this,a,2,b)}
u(te,re);te.prototype.j=function(a){this.Fc(a,Ja.apply(1,arguments))};function ue(a){a&&"function"==typeof a.dispose&&a.dispose()}
;function ve(a){for(var b=0,c=arguments.length;b<c;++b){var d=arguments[b];Pa(d)?ve.apply(null,d):ue(d)}}
;function J(){this.M=this.M;this.v=this.v}
J.prototype.M=!1;J.prototype.h=function(){return this.M};
J.prototype.dispose=function(){this.M||(this.M=!0,this.C())};
function we(a,b){xe(a,Ya(ue,b))}
function xe(a,b){a.M?b():(a.v||(a.v=[]),a.v.push(b))}
J.prototype.C=function(){if(this.v)for(;this.v.length;)this.v.shift()()};function ye(a,b){this.type=a;this.h=this.target=b;this.defaultPrevented=this.j=!1}
ye.prototype.stopPropagation=function(){this.j=!0};
ye.prototype.preventDefault=function(){this.defaultPrevented=!0};function ze(a){var b=B("window.location.href");null==a&&(a='Unknown Error of type "null/undefined"');if("string"===typeof a)return{message:a,name:"Unknown error",lineNumber:"Not available",fileName:b,stack:"Not available"};var c=!1;try{var d=a.lineNumber||a.line||"Not available"}catch(g){d="Not available",c=!0}try{var e=a.fileName||a.filename||a.sourceURL||z.$googDebugFname||b}catch(g){e="Not available",c=!0}b=Ae(a);if(!(!c&&a.lineNumber&&a.fileName&&a.stack&&a.message&&a.name)){c=a.message;if(null==
c){if(a.constructor&&a.constructor instanceof Function){if(a.constructor.name)c=a.constructor.name;else if(c=a.constructor,Be[c])c=Be[c];else{c=String(c);if(!Be[c]){var f=/function\s+([^\(]+)/m.exec(c);Be[c]=f?f[1]:"[Anonymous]"}c=Be[c]}c='Unknown Error of type "'+c+'"'}else c="Unknown Error of unknown type";"function"===typeof a.toString&&Object.prototype.toString!==a.toString&&(c+=": "+a.toString())}return{message:c,name:a.name||"UnknownError",lineNumber:d,fileName:e,stack:b||"Not available"}}a.stack=
b;return{message:a.message,name:a.name,lineNumber:a.lineNumber,fileName:a.fileName,stack:a.stack}}
function Ae(a,b){b||(b={});b[He(a)]=!0;var c=a.stack||"";(a=a.cause)&&!b[He(a)]&&(c+="\nCaused by: ",a.stack&&0==a.stack.indexOf(a.toString())||(c+="string"===typeof a?a:a.message+"\n"),c+=Ae(a,b));return c}
function He(a){var b="";"function"===typeof a.toString&&(b=""+a);return b+a.stack}
var Be={};var Ie=function(){if(!z.addEventListener||!Object.defineProperty)return!1;var a=!1,b=Object.defineProperty({},"passive",{get:function(){a=!0}});
try{z.addEventListener("test",function(){},b),z.removeEventListener("test",function(){},b)}catch(c){}return a}();function Je(a,b){ye.call(this,a?a.type:"");this.relatedTarget=this.h=this.target=null;this.button=this.screenY=this.screenX=this.clientY=this.clientX=0;this.key="";this.charCode=this.keyCode=0;this.metaKey=this.shiftKey=this.altKey=this.ctrlKey=!1;this.state=null;this.pointerId=0;this.pointerType="";this.i=null;a&&this.init(a,b)}
Za(Je,ye);var Ke={2:"touch",3:"pen",4:"mouse"};
Je.prototype.init=function(a,b){var c=this.type=a.type,d=a.changedTouches&&a.changedTouches.length?a.changedTouches[0]:null;this.target=a.target||a.srcElement;this.h=b;if(b=a.relatedTarget){if(Kc){a:{try{Gc(b.nodeName);var e=!0;break a}catch(f){}e=!1}e||(b=null)}}else"mouseover"==c?b=a.fromElement:"mouseout"==c&&(b=a.toElement);this.relatedTarget=b;d?(this.clientX=void 0!==d.clientX?d.clientX:d.pageX,this.clientY=void 0!==d.clientY?d.clientY:d.pageY,this.screenX=d.screenX||0,this.screenY=d.screenY||
0):(this.clientX=void 0!==a.clientX?a.clientX:a.pageX,this.clientY=void 0!==a.clientY?a.clientY:a.pageY,this.screenX=a.screenX||0,this.screenY=a.screenY||0);this.button=a.button;this.keyCode=a.keyCode||0;this.key=a.key||"";this.charCode=a.charCode||("keypress"==c?a.keyCode:0);this.ctrlKey=a.ctrlKey;this.altKey=a.altKey;this.shiftKey=a.shiftKey;this.metaKey=a.metaKey;this.pointerId=a.pointerId||0;this.pointerType="string"===typeof a.pointerType?a.pointerType:Ke[a.pointerType]||"";this.state=a.state;
this.i=a;a.defaultPrevented&&Je.X.preventDefault.call(this)};
Je.prototype.stopPropagation=function(){Je.X.stopPropagation.call(this);this.i.stopPropagation?this.i.stopPropagation():this.i.cancelBubble=!0};
Je.prototype.preventDefault=function(){Je.X.preventDefault.call(this);var a=this.i;a.preventDefault?a.preventDefault():a.returnValue=!1};var Le="closure_listenable_"+(1E6*Math.random()|0);var Me=0;function Ne(a,b,c,d,e){this.listener=a;this.proxy=null;this.src=b;this.type=c;this.capture=!!d;this.hb=e;this.key=++Me;this.Pa=this.cb=!1}
function Oe(a){a.Pa=!0;a.listener=null;a.proxy=null;a.src=null;a.hb=null}
;function Pe(a){this.src=a;this.listeners={};this.h=0}
Pe.prototype.add=function(a,b,c,d,e){var f=a.toString();a=this.listeners[f];a||(a=this.listeners[f]=[],this.h++);var g=Qe(a,b,d,e);-1<g?(b=a[g],c||(b.cb=!1)):(b=new Ne(b,this.src,f,!!d,e),b.cb=c,a.push(b));return b};
Pe.prototype.remove=function(a,b,c,d){a=a.toString();if(!(a in this.listeners))return!1;var e=this.listeners[a];b=Qe(e,b,c,d);return-1<b?(Oe(e[b]),Array.prototype.splice.call(e,b,1),0==e.length&&(delete this.listeners[a],this.h--),!0):!1};
function Re(a,b){var c=b.type;c in a.listeners&&kb(a.listeners[c],b)&&(Oe(b),0==a.listeners[c].length&&(delete a.listeners[c],a.h--))}
function Qe(a,b,c,d){for(var e=0;e<a.length;++e){var f=a[e];if(!f.Pa&&f.listener==b&&f.capture==!!c&&f.hb==d)return e}return-1}
;var Se="closure_lm_"+(1E6*Math.random()|0),Te={},Ue=0;function Ve(a,b,c,d,e){if(d&&d.once)We(a,b,c,d,e);else if(Array.isArray(b))for(var f=0;f<b.length;f++)Ve(a,b[f],c,d,e);else c=Xe(c),a&&a[Le]?a.ha(b,c,Qa(d)?!!d.capture:!!d,e):Ye(a,b,c,!1,d,e)}
function Ye(a,b,c,d,e,f){if(!b)throw Error("Invalid event type");var g=Qa(e)?!!e.capture:!!e,h=Ze(a);h||(a[Se]=h=new Pe(a));c=h.add(b,c,d,g,f);if(!c.proxy){d=$e();c.proxy=d;d.src=a;d.listener=c;if(a.addEventListener)Ie||(e=g),void 0===e&&(e=!1),a.addEventListener(b.toString(),d,e);else if(a.attachEvent)a.attachEvent(af(b.toString()),d);else if(a.addListener&&a.removeListener)a.addListener(d);else throw Error("addEventListener and attachEvent are unavailable.");Ue++}}
function $e(){function a(c){return b.call(a.src,a.listener,c)}
var b=bf;return a}
function We(a,b,c,d,e){if(Array.isArray(b))for(var f=0;f<b.length;f++)We(a,b[f],c,d,e);else c=Xe(c),a&&a[Le]?a.l.add(String(b),c,!0,Qa(d)?!!d.capture:!!d,e):Ye(a,b,c,!0,d,e)}
function cf(a,b,c,d,e){if(Array.isArray(b))for(var f=0;f<b.length;f++)cf(a,b[f],c,d,e);else(d=Qa(d)?!!d.capture:!!d,c=Xe(c),a&&a[Le])?a.l.remove(String(b),c,d,e):a&&(a=Ze(a))&&(b=a.listeners[b.toString()],a=-1,b&&(a=Qe(b,c,d,e)),(c=-1<a?b[a]:null)&&df(c))}
function df(a){if("number"!==typeof a&&a&&!a.Pa){var b=a.src;if(b&&b[Le])Re(b.l,a);else{var c=a.type,d=a.proxy;b.removeEventListener?b.removeEventListener(c,d,a.capture):b.detachEvent?b.detachEvent(af(c),d):b.addListener&&b.removeListener&&b.removeListener(d);Ue--;(c=Ze(b))?(Re(c,a),0==c.h&&(c.src=null,b[Se]=null)):Oe(a)}}}
function af(a){return a in Te?Te[a]:Te[a]="on"+a}
function bf(a,b){if(a.Pa)a=!0;else{b=new Je(b,this);var c=a.listener,d=a.hb||a.src;a.cb&&df(a);a=c.call(d,b)}return a}
function Ze(a){a=a[Se];return a instanceof Pe?a:null}
var ef="__closure_events_fn_"+(1E9*Math.random()>>>0);function Xe(a){if("function"===typeof a)return a;a[ef]||(a[ef]=function(b){return a.handleEvent(b)});
return a[ef]}
;function ff(){J.call(this);this.l=new Pe(this);this.Cc=this;this.ja=null}
Za(ff,J);ff.prototype[Le]=!0;ff.prototype.addEventListener=function(a,b,c,d){Ve(this,a,b,c,d)};
ff.prototype.removeEventListener=function(a,b,c,d){cf(this,a,b,c,d)};
function gf(a,b){var c=a.ja;if(c){var d=[];for(var e=1;c;c=c.ja)d.push(c),++e}a=a.Cc;c=b.type||b;"string"===typeof b?b=new ye(b,a):b instanceof ye?b.target=b.target||a:(e=b,b=new ye(c,a),xb(b,e));e=!0;if(d)for(var f=d.length-1;!b.j&&0<=f;f--){var g=b.h=d[f];e=hf(g,c,!0,b)&&e}b.j||(g=b.h=a,e=hf(g,c,!0,b)&&e,b.j||(e=hf(g,c,!1,b)&&e));if(d)for(f=0;!b.j&&f<d.length;f++)g=b.h=d[f],e=hf(g,c,!1,b)&&e}
ff.prototype.C=function(){ff.X.C.call(this);if(this.l){var a=this.l,b=0,c;for(c in a.listeners){for(var d=a.listeners[c],e=0;e<d.length;e++)++b,Oe(d[e]);delete a.listeners[c];a.h--}}this.ja=null};
ff.prototype.ha=function(a,b,c,d){return this.l.add(String(a),b,!1,c,d)};
function hf(a,b,c,d){b=a.l.listeners[String(b)];if(!b)return!0;b=b.concat();for(var e=!0,f=0;f<b.length;++f){var g=b[f];if(g&&!g.Pa&&g.capture==c){var h=g.listener,k=g.hb||g.src;g.cb&&Re(a.l,g);e=!1!==h.call(k,d)&&e}}return e&&!d.defaultPrevented}
;function jf(a,b){this.j=a;this.l=b;this.i=0;this.h=null}
jf.prototype.get=function(){if(0<this.i){this.i--;var a=this.h;this.h=a.next;a.next=null}else a=this.j();return a};
function kf(a,b){a.l(b);100>a.i&&(a.i++,b.next=a.h,a.h=b)}
;function lf(a,b){return a+Math.random()*(b-a)}
;function mf(a,b){this.x=void 0!==a?a:0;this.y=void 0!==b?b:0}
l=mf.prototype;l.clone=function(){return new mf(this.x,this.y)};
l.equals=function(a){return a instanceof mf&&(this==a?!0:this&&a?this.x==a.x&&this.y==a.y:!1)};
l.ceil=function(){this.x=Math.ceil(this.x);this.y=Math.ceil(this.y);return this};
l.floor=function(){this.x=Math.floor(this.x);this.y=Math.floor(this.y);return this};
l.round=function(){this.x=Math.round(this.x);this.y=Math.round(this.y);return this};
l.scale=function(a,b){this.x*=a;this.y*="number"===typeof b?b:a;return this};function nf(a,b){this.width=a;this.height=b}
l=nf.prototype;l.clone=function(){return new nf(this.width,this.height)};
l.aspectRatio=function(){return this.width/this.height};
l.Oa=function(){return!(this.width*this.height)};
l.ceil=function(){this.width=Math.ceil(this.width);this.height=Math.ceil(this.height);return this};
l.floor=function(){this.width=Math.floor(this.width);this.height=Math.floor(this.height);return this};
l.round=function(){this.width=Math.round(this.width);this.height=Math.round(this.height);return this};
l.scale=function(a,b){this.width*=a;this.height*="number"===typeof b?b:a;return this};function of(a){var b=document;return"string"===typeof a?b.getElementById(a):a}
function pf(a){var b=document;a=String(a);"application/xhtml+xml"===b.contentType&&(a=a.toLowerCase());return b.createElement(a)}
function qf(a,b){for(var c=0;a;){if(b(a))return a;a=a.parentNode;c++}return null}
;var rf;function sf(){var a=z.MessageChannel;"undefined"===typeof a&&"undefined"!==typeof window&&window.postMessage&&window.addEventListener&&!C("Presto")&&(a=function(){var e=pf("IFRAME");e.style.display="none";document.documentElement.appendChild(e);var f=e.contentWindow;e=f.document;e.open();e.close();var g="callImmediate"+Math.random(),h="file:"==f.location.protocol?"*":f.location.protocol+"//"+f.location.host;e=Xa(function(k){if(("*"==h||k.origin==h)&&k.data==g)this.port1.onmessage()},this);
f.addEventListener("message",e,!1);this.port1={};this.port2={postMessage:function(){f.postMessage(g,h)}}});
if("undefined"!==typeof a&&!Tb()){var b=new a,c={},d=c;b.port1.onmessage=function(){if(void 0!==c.next){c=c.next;var e=c.Rb;c.Rb=null;e()}};
return function(e){d.next={Rb:e};d=d.next;b.port2.postMessage(0)}}return function(e){z.setTimeout(e,0)}}
;function tf(){this.i=this.h=null}
tf.prototype.add=function(a,b){var c=uf.get();c.set(a,b);this.i?this.i.next=c:this.h=c;this.i=c};
tf.prototype.remove=function(){var a=null;this.h&&(a=this.h,this.h=this.h.next,this.h||(this.i=null),a.next=null);return a};
var uf=new jf(function(){return new vf},function(a){return a.reset()});
function vf(){this.next=this.scope=this.h=null}
vf.prototype.set=function(a,b){this.h=a;this.scope=b;this.next=null};
vf.prototype.reset=function(){this.next=this.scope=this.h=null};var wf,xf=!1,yf=new tf;function zf(a,b){wf||Af();xf||(wf(),xf=!0);yf.add(a,b)}
function Af(){if(z.Promise&&z.Promise.resolve){var a=z.Promise.resolve(void 0);wf=function(){a.then(Bf)}}else wf=function(){var b=Bf;
"function"!==typeof z.setImmediate||z.Window&&z.Window.prototype&&!C("Edge")&&z.Window.prototype.setImmediate==z.setImmediate?(rf||(rf=sf()),rf(b)):z.setImmediate(b)}}
function Bf(){for(var a;a=yf.remove();){try{a.h.call(a.scope)}catch(b){Dc(b)}kf(uf,a)}xf=!1}
;function Cf(a){this.h=0;this.v=void 0;this.l=this.i=this.j=null;this.m=this.o=!1;if(a!=cb)try{var b=this;a.call(void 0,function(c){Df(b,2,c)},function(c){Df(b,3,c)})}catch(c){Df(this,3,c)}}
function Ef(){this.next=this.context=this.i=this.j=this.h=null;this.l=!1}
Ef.prototype.reset=function(){this.context=this.i=this.j=this.h=null;this.l=!1};
var Ff=new jf(function(){return new Ef},function(a){a.reset()});
function Gf(a,b,c){var d=Ff.get();d.j=a;d.i=b;d.context=c;return d}
function Hf(a){return new Cf(function(b,c){c(a)})}
Cf.prototype.then=function(a,b,c){return If(this,"function"===typeof a?a:null,"function"===typeof b?b:null,c)};
Cf.prototype.$goog_Thenable=!0;l=Cf.prototype;l.qb=function(a,b){return If(this,null,a,b)};
l.catch=Cf.prototype.qb;l.cancel=function(a){if(0==this.h){var b=new Jf(a);zf(function(){Kf(this,b)},this)}};
function Kf(a,b){if(0==a.h)if(a.j){var c=a.j;if(c.i){for(var d=0,e=null,f=null,g=c.i;g&&(g.l||(d++,g.h==a&&(e=g),!(e&&1<d)));g=g.next)e||(f=g);e&&(0==c.h&&1==d?Kf(c,b):(f?(d=f,d.next==c.l&&(c.l=d),d.next=d.next.next):Lf(c),Mf(c,e,3,b)))}a.j=null}else Df(a,3,b)}
function Nf(a,b){a.i||2!=a.h&&3!=a.h||Of(a);a.l?a.l.next=b:a.i=b;a.l=b}
function If(a,b,c,d){var e=Gf(null,null,null);e.h=new Cf(function(f,g){e.j=b?function(h){try{var k=b.call(d,h);f(k)}catch(m){g(m)}}:f;
e.i=c?function(h){try{var k=c.call(d,h);void 0===k&&h instanceof Jf?g(h):f(k)}catch(m){g(m)}}:g});
e.h.j=a;Nf(a,e);return e.h}
l.Ad=function(a){this.h=0;Df(this,2,a)};
l.Bd=function(a){this.h=0;Df(this,3,a)};
function Df(a,b,c){if(0==a.h){a===c&&(b=3,c=new TypeError("Promise cannot resolve to itself"));a.h=1;a:{var d=c,e=a.Ad,f=a.Bd;if(d instanceof Cf){Nf(d,Gf(e||cb,f||null,a));var g=!0}else{if(d)try{var h=!!d.$goog_Thenable}catch(m){h=!1}else h=!1;if(h)d.then(e,f,a),g=!0;else{if(Qa(d))try{var k=d.then;if("function"===typeof k){Pf(d,k,e,f,a);g=!0;break a}}catch(m){f.call(a,m);g=!0;break a}g=!1}}}g||(a.v=c,a.h=b,a.j=null,Of(a),3!=b||c instanceof Jf||Qf(a,c))}}
function Pf(a,b,c,d,e){function f(k){h||(h=!0,d.call(e,k))}
function g(k){h||(h=!0,c.call(e,k))}
var h=!1;try{b.call(a,g,f)}catch(k){f(k)}}
function Of(a){a.o||(a.o=!0,zf(a.Pc,a))}
function Lf(a){var b=null;a.i&&(b=a.i,a.i=b.next,b.next=null);a.i||(a.l=null);return b}
l.Pc=function(){for(var a;a=Lf(this);)Mf(this,a,this.h,this.v);this.o=!1};
function Mf(a,b,c,d){if(3==c&&b.i&&!b.l)for(;a&&a.m;a=a.j)a.m=!1;if(b.h)b.h.j=null,Rf(b,c,d);else try{b.l?b.j.call(b.context):Rf(b,c,d)}catch(e){Sf.call(null,e)}kf(Ff,b)}
function Rf(a,b,c){2==b?a.j.call(a.context,c):a.i&&a.i.call(a.context,c)}
function Qf(a,b){a.m=!0;zf(function(){a.m&&Sf.call(null,b)})}
var Sf=Dc;function Jf(a){ab.call(this,a)}
Za(Jf,ab);Jf.prototype.name="cancel";function Tf(a,b){ff.call(this);this.j=a||1;this.i=b||z;this.m=Xa(this.yd,this);this.o=Date.now()}
Za(Tf,ff);l=Tf.prototype;l.enabled=!1;l.aa=null;function Uf(a,b){a.j=b;a.aa&&a.enabled?(a.stop(),a.start()):a.aa&&a.stop()}
l.yd=function(){if(this.enabled){var a=Date.now()-this.o;0<a&&a<.8*this.j?this.aa=this.i.setTimeout(this.m,this.j-a):(this.aa&&(this.i.clearTimeout(this.aa),this.aa=null),gf(this,"tick"),this.enabled&&(this.stop(),this.start()))}};
l.start=function(){this.enabled=!0;this.aa||(this.aa=this.i.setTimeout(this.m,this.j),this.o=Date.now())};
l.stop=function(){this.enabled=!1;this.aa&&(this.i.clearTimeout(this.aa),this.aa=null)};
l.C=function(){Tf.X.C.call(this);this.stop();delete this.i};
function Vf(a,b,c){if("function"===typeof a)c&&(a=Xa(a,c));else if(a&&"function"==typeof a.handleEvent)a=Xa(a.handleEvent,a);else throw Error("Invalid listener argument");return 2147483647<Number(b)?-1:z.setTimeout(a,b||0)}
;function Wf(a){this.v=a;this.h=new Map;this.o=new Set;this.j=0;this.l=100;this.flushInterval=3E4;this.i=new Tf(this.flushInterval);this.i.ha("tick",this.tb,!1,this);this.m=!1}
l=Wf.prototype;l.pc=function(a){this.m=a;this.l=1};
function Xf(a){a.i.enabled||a.i.start();a.j++;a.j>=a.l&&a.tb()}
l.tb=function(){var a=this.h.values();a=[].concat(ia(a)).filter(function(b){return b.Aa.size});
a.length&&this.v.flush(a,this.m);Yf(a);this.j=0;this.i.enabled&&this.i.stop()};
l.Ob=function(a){var b=Ja.apply(1,arguments);this.h.has(a)||this.h.set(a,new se(a,b))};
l.Pb=function(a){var b=Ja.apply(1,arguments);this.h.has(a)||this.h.set(a,new te(a,b))};
function Zf(a,b){return a.o.has(b)?void 0:a.h.get(b)}
l.rb=function(a){this.Ac.apply(this,[a,1].concat(ia(Ja.apply(1,arguments))))};
l.Ac=function(a,b){var c=Ja.apply(2,arguments),d=Zf(this,a);d&&d instanceof se&&(d.j(b,c),Xf(this))};
l.sb=function(a,b){var c=Ja.apply(2,arguments),d=Zf(this,a);d&&d instanceof te&&(d.j(b,c),Xf(this))};
function Yf(a){for(var b=0;b<a.length;b++)a[b].clear()}
;function $f(a){this.h=a;this.h.Ob("/client_streamz/bg/fiec",{Ma:3,La:"rk"},{Ma:2,La:"ec"})}
function ag(a){this.h=a;this.h.Pb("/client_streamz/bg/fil",{Ma:3,La:"rk"})}
function bg(a){this.h=a;this.h.Ob("/client_streamz/bg/fsc",{Ma:3,La:"rk"})}
function cg(a){this.h=a;this.h.Pb("/client_streamz/bg/fsl",{Ma:3,La:"rk"})}
;function dg(a){H.call(this,a,-1,eg)}
u(dg,H);function fg(a){H.call(this,a,-1,gg)}
u(fg,H);function hg(a){H.call(this,a)}
u(hg,H);function ig(a){H.call(this,a)}
u(ig,H);var eg=[3,6,4],gg=[1],jg=[1,2,3],kg=[1,2,3];function lg(a){H.call(this,a,-1,mg)}
u(lg,H);var mg=[1];function ng(a){if(!a)return"";if(/^about:(?:blank|srcdoc)$/.test(a))return window.origin||"";a=a.split("#")[0].split("?")[0];a=a.toLowerCase();0==a.indexOf("//")&&(a=window.location.protocol+a);/^[\w\-]*:\/\//.test(a)||(a=window.location.href);var b=a.substring(a.indexOf("://")+3),c=b.indexOf("/");-1!=c&&(b=b.substring(0,c));c=a.substring(0,a.indexOf("://"));if(!c)throw Error("URI is missing protocol: "+a);if("http"!==c&&"https"!==c&&"chrome-extension"!==c&&"moz-extension"!==c&&"file"!==c&&"android-app"!==
c&&"chrome-search"!==c&&"chrome-untrusted"!==c&&"chrome"!==c&&"app"!==c&&"devtools"!==c)throw Error("Invalid URI scheme in origin: "+c);a="";var d=b.indexOf(":");if(-1!=d){var e=b.substring(d+1);b=b.substring(0,d);if("http"===c&&"80"!==e||"https"===c&&"443"!==e)a=":"+e}return c+"://"+b+a}
;function og(){function a(){e[0]=1732584193;e[1]=4023233417;e[2]=2562383102;e[3]=271733878;e[4]=3285377520;q=m=0}
function b(r){for(var w=g,t=0;64>t;t+=4)w[t/4]=r[t]<<24|r[t+1]<<16|r[t+2]<<8|r[t+3];for(t=16;80>t;t++)r=w[t-3]^w[t-8]^w[t-14]^w[t-16],w[t]=(r<<1|r>>>31)&4294967295;r=e[0];var y=e[1],E=e[2],F=e[3],O=e[4];for(t=0;80>t;t++){if(40>t)if(20>t){var N=F^y&(E^F);var Q=1518500249}else N=y^E^F,Q=1859775393;else 60>t?(N=y&E|F&(y|E),Q=2400959708):(N=y^E^F,Q=3395469782);N=((r<<5|r>>>27)&4294967295)+N+O+Q+w[t]&4294967295;O=F;F=E;E=(y<<30|y>>>2)&4294967295;y=r;r=N}e[0]=e[0]+r&4294967295;e[1]=e[1]+y&4294967295;e[2]=
e[2]+E&4294967295;e[3]=e[3]+F&4294967295;e[4]=e[4]+O&4294967295}
function c(r,w){if("string"===typeof r){r=unescape(encodeURIComponent(r));for(var t=[],y=0,E=r.length;y<E;++y)t.push(r.charCodeAt(y));r=t}w||(w=r.length);t=0;if(0==m)for(;t+64<w;)b(r.slice(t,t+64)),t+=64,q+=64;for(;t<w;)if(f[m++]=r[t++],q++,64==m)for(m=0,b(f);t+64<w;)b(r.slice(t,t+64)),t+=64,q+=64}
function d(){var r=[],w=8*q;56>m?c(h,56-m):c(h,64-(m-56));for(var t=63;56<=t;t--)f[t]=w&255,w>>>=8;b(f);for(t=w=0;5>t;t++)for(var y=24;0<=y;y-=8)r[w++]=e[t]>>y&255;return r}
for(var e=[],f=[],g=[],h=[128],k=1;64>k;++k)h[k]=0;var m,q;a();return{reset:a,update:c,digest:d,Mc:function(){for(var r=d(),w="",t=0;t<r.length;t++)w+="0123456789ABCDEF".charAt(Math.floor(r[t]/16))+"0123456789ABCDEF".charAt(r[t]%16);return w}}}
;function pg(a,b,c){var d=String(z.location.href);return d&&a&&b?[b,qg(ng(d),a,c||null)].join(" "):null}
function qg(a,b,c){var d=[],e=[];if(1==(Array.isArray(c)?2:1))return e=[b,a],fb(d,function(h){e.push(h)}),rg(e.join(" "));
var f=[],g=[];fb(c,function(h){g.push(h.key);f.push(h.value)});
c=Math.floor((new Date).getTime()/1E3);e=0==f.length?[c,b,a]:[f.join(":"),c,b,a];fb(d,function(h){e.push(h)});
a=rg(e.join(" "));a=[c,a];0==g.length||a.push(g.join(""));return a.join("_")}
function rg(a){var b=og();b.update(a);return b.Mc().toLowerCase()}
;var sg={};function tg(a){this.h=a||{cookie:""}}
l=tg.prototype;l.isEnabled=function(){if(!z.navigator.cookieEnabled)return!1;if(!this.Oa())return!0;this.set("TESTCOOKIESENABLED","1",{ib:60});if("1"!==this.get("TESTCOOKIESENABLED"))return!1;this.remove("TESTCOOKIESENABLED");return!0};
l.set=function(a,b,c){var d=!1;if("object"===typeof c){var e=c.Dr;d=c.secure||!1;var f=c.domain||void 0;var g=c.path||void 0;var h=c.ib}if(/[;=\s]/.test(a))throw Error('Invalid cookie name "'+a+'"');if(/[;\r\n]/.test(b))throw Error('Invalid cookie value "'+b+'"');void 0===h&&(h=-1);c=f?";domain="+f:"";g=g?";path="+g:"";d=d?";secure":"";h=0>h?"":0==h?";expires="+(new Date(1970,1,1)).toUTCString():";expires="+(new Date(Date.now()+1E3*h)).toUTCString();this.h.cookie=a+"="+b+c+g+h+d+(null!=e?";samesite="+
e:"")};
l.get=function(a,b){for(var c=a+"=",d=(this.h.cookie||"").split(";"),e=0,f;e<d.length;e++){f=Lb(d[e]);if(0==f.lastIndexOf(c,0))return f.slice(c.length);if(f==a)return""}return b};
l.remove=function(a,b,c){var d=void 0!==this.get(a);this.set(a,"",{ib:0,path:b,domain:c});return d};
l.xb=function(){return ug(this).keys};
l.Oa=function(){return!this.h.cookie};
l.clear=function(){for(var a=ug(this).keys,b=a.length-1;0<=b;b--)this.remove(a[b])};
function ug(a){a=(a.h.cookie||"").split(";");for(var b=[],c=[],d,e,f=0;f<a.length;f++)e=Lb(a[f]),d=e.indexOf("="),-1==d?(b.push(""),c.push(e)):(b.push(e.substring(0,d)),c.push(e.substring(d+1)));return{keys:b,values:c}}
var vg=new tg("undefined"==typeof document?null:document);function wg(a){return!!sg.FPA_SAMESITE_PHASE2_MOD||!(void 0===a||!a)}
function xg(a){a=void 0===a?!1:a;var b=z.__SAPISID||z.__APISID||z.__3PSAPISID||z.__OVERRIDE_SID;wg(a)&&(b=b||z.__1PSAPISID);if(b)return!0;var c=new tg(document);b=c.get("SAPISID")||c.get("APISID")||c.get("__Secure-3PAPISID")||c.get("SID");wg(a)&&(b=b||c.get("__Secure-1PAPISID"));return!!b}
function yg(a,b,c,d){(a=z[a])||(a=(new tg(document)).get(b));return a?pg(a,c,d):null}
function zg(a,b){b=void 0===b?!1:b;var c=ng(String(z.location.href)),d=[];if(xg(b)){c=0==c.indexOf("https:")||0==c.indexOf("chrome-extension:")||0==c.indexOf("moz-extension:");var e=c?z.__SAPISID:z.__APISID;e||(e=new tg(document),e=e.get(c?"SAPISID":"APISID")||e.get("__Secure-3PAPISID"));(e=e?pg(e,c?"SAPISIDHASH":"APISIDHASH",a):null)&&d.push(e);c&&wg(b)&&((b=yg("__1PSAPISID","__Secure-1PAPISID","SAPISID1PHASH",a))&&d.push(b),(a=yg("__3PSAPISID","__Secure-3PAPISID","SAPISID3PHASH",a))&&d.push(a))}return 0==
d.length?null:d.join(" ")}
;function Ag(a){H.call(this,a,-1,Bg)}
u(Ag,H);var Bg=[2];function Cg(a){this.h=this.i=this.j=a}
Cg.prototype.reset=function(){this.h=this.i=this.j};
Cg.prototype.getValue=function(){return this.i};function Dg(a){var b=[];Eg(new Fg,a,b);return b.join("")}
function Fg(){}
function Eg(a,b,c){if(null==b)c.push("null");else{if("object"==typeof b){if(Array.isArray(b)){var d=b;b=d.length;c.push("[");for(var e="",f=0;f<b;f++)c.push(e),Eg(a,d[f],c),e=",";c.push("]");return}if(b instanceof String||b instanceof Number||b instanceof Boolean)b=b.valueOf();else{c.push("{");e="";for(d in b)Object.prototype.hasOwnProperty.call(b,d)&&(f=b[d],"function"!=typeof f&&(c.push(e),Gg(d,c),c.push(":"),Eg(a,f,c),e=","));c.push("}");return}}switch(typeof b){case "string":Gg(b,c);break;case "number":c.push(isFinite(b)&&
!isNaN(b)?String(b):"null");break;case "boolean":c.push(String(b));break;case "function":c.push("null");break;default:throw Error("Unknown type: "+typeof b);}}}
var Hg={'"':'\\"',"\\":"\\\\","/":"\\/","\b":"\\b","\f":"\\f","\n":"\\n","\r":"\\r","\t":"\\t","\v":"\\u000b"},Ig=/\uffff/.test("\uffff")?/[\\"\x00-\x1f\x7f-\uffff]/g:/[\\"\x00-\x1f\x7f-\xff]/g;function Gg(a,b){b.push('"',a.replace(Ig,function(c){var d=Hg[c];d||(d="\\u"+(c.charCodeAt(0)|65536).toString(16).slice(1),Hg[c]=d);return d}),'"')}
;function Jg(){}
Jg.prototype.h=null;Jg.prototype.getOptions=function(){var a;(a=this.h)||(a={},Kg(this)&&(a[0]=!0,a[1]=!0),a=this.h=a);return a};var Lg;function Mg(){}
Za(Mg,Jg);function Ng(a){return(a=Kg(a))?new ActiveXObject(a):new XMLHttpRequest}
function Kg(a){if(!a.i&&"undefined"==typeof XMLHttpRequest&&"undefined"!=typeof ActiveXObject){for(var b=["MSXML2.XMLHTTP.6.0","MSXML2.XMLHTTP.3.0","MSXML2.XMLHTTP","Microsoft.XMLHTTP"],c=0;c<b.length;c++){var d=b[c];try{return new ActiveXObject(d),a.i=d}catch(e){}}throw Error("Could not create ActiveXObject. ActiveX might be disabled, or MSXML might not be installed");}return a.i}
Lg=new Mg;function Og(a){ff.call(this);this.headers=new Map;this.K=a||null;this.i=!1;this.I=this.A=null;this.m=this.S="";this.j=this.P=this.s=this.O=!1;this.o=0;this.F=null;this.ca="";this.V=this.W=!1}
Za(Og,ff);var Rg=/^https?$/i,Sg=["POST","PUT"],Tg=[];function Ug(a,b,c,d,e,f,g){var h=new Og;Tg.push(h);b&&h.ha("complete",b);h.l.add("ready",h.Kc,!0,void 0,void 0);f&&(h.o=Math.max(0,f));g&&(h.W=g);h.send(a,c,d,e)}
l=Og.prototype;l.Kc=function(){this.dispose();kb(Tg,this)};
l.send=function(a,b,c,d){if(this.A)throw Error("[goog.net.XhrIo] Object is active with another request="+this.S+"; newUri="+a);b=b?b.toUpperCase():"GET";this.S=a;this.m="";this.O=!1;this.i=!0;this.A=this.K?Ng(this.K):Ng(Lg);this.I=this.K?this.K.getOptions():Lg.getOptions();this.A.onreadystatechange=Xa(this.ic,this);try{this.getStatus(),this.P=!0,this.A.open(b,String(a),!0),this.P=!1}catch(g){this.getStatus();Vg(this,g);return}a=c||"";c=new Map(this.headers);if(d)if(Object.getPrototypeOf(d)===Object.prototype)for(var e in d)c.set(e,
d[e]);else if("function"===typeof d.keys&&"function"===typeof d.get){e=p(d.keys());for(var f=e.next();!f.done;f=e.next())f=f.value,c.set(f,d.get(f))}else throw Error("Unknown input type for opt_headers: "+String(d));d=Array.from(c.keys()).find(function(g){return"content-type"==g.toLowerCase()});
e=z.FormData&&a instanceof z.FormData;!(0<=eb(Sg,b))||d||e||c.set("Content-Type","application/x-www-form-urlencoded;charset=utf-8");b=p(c);for(d=b.next();!d.done;d=b.next())c=p(d.value),d=c.next().value,c=c.next().value,this.A.setRequestHeader(d,c);this.ca&&(this.A.responseType=this.ca);"withCredentials"in this.A&&this.A.withCredentials!==this.W&&(this.A.withCredentials=this.W);try{Wg(this),0<this.o&&(this.V=Xg(this.A),this.getStatus(),this.V?(this.A.timeout=this.o,this.A.ontimeout=Xa(this.vc,this)):
this.F=Vf(this.vc,this.o,this)),this.getStatus(),this.s=!0,this.A.send(a),this.s=!1}catch(g){this.getStatus(),Vg(this,g)}};
function Xg(a){return Ic&&"number"===typeof a.timeout&&void 0!==a.ontimeout}
l.vc=function(){"undefined"!=typeof Ma&&this.A&&(this.m="Timed out after "+this.o+"ms, aborting",this.getStatus(),gf(this,"timeout"),this.abort(8))};
function Vg(a,b){a.i=!1;a.A&&(a.j=!0,a.A.abort(),a.j=!1);a.m=b;Yg(a);Zg(a)}
function Yg(a){a.O||(a.O=!0,gf(a,"complete"),gf(a,"error"))}
l.abort=function(){this.A&&this.i&&(this.getStatus(),this.i=!1,this.j=!0,this.A.abort(),this.j=!1,gf(this,"complete"),gf(this,"abort"),Zg(this))};
l.C=function(){this.A&&(this.i&&(this.i=!1,this.j=!0,this.A.abort(),this.j=!1),Zg(this,!0));Og.X.C.call(this)};
l.ic=function(){this.h()||(this.P||this.s||this.j?$g(this):this.cd())};
l.cd=function(){$g(this)};
function $g(a){if(a.i&&"undefined"!=typeof Ma)if(a.I[1]&&4==ah(a)&&2==a.getStatus())a.getStatus();else if(a.s&&4==ah(a))Vf(a.ic,0,a);else if(gf(a,"readystatechange"),a.isComplete()){a.getStatus();a.i=!1;try{if(bh(a))gf(a,"complete"),gf(a,"success");else{try{var b=2<ah(a)?a.A.statusText:""}catch(c){b=""}a.m=b+" ["+a.getStatus()+"]";Yg(a)}}finally{Zg(a)}}}
function Zg(a,b){if(a.A){Wg(a);var c=a.A,d=a.I[0]?function(){}:null;
a.A=null;a.I=null;b||gf(a,"ready");try{c.onreadystatechange=d}catch(e){}}}
function Wg(a){a.A&&a.V&&(a.A.ontimeout=null);a.F&&(z.clearTimeout(a.F),a.F=null)}
l.isActive=function(){return!!this.A};
l.isComplete=function(){return 4==ah(this)};
function bh(a){var b=a.getStatus();a:switch(b){case 200:case 201:case 202:case 204:case 206:case 304:case 1223:var c=!0;break a;default:c=!1}if(!c){if(b=0===b)a=mc(1,String(a.S)),!a&&z.self&&z.self.location&&(a=z.self.location.protocol.slice(0,-1)),b=!Rg.test(a?a.toLowerCase():"");c=b}return c}
function ah(a){return a.A?a.A.readyState:0}
l.getStatus=function(){try{return 2<ah(this)?this.A.status:-1}catch(a){return-1}};
l.getLastError=function(){return"string"===typeof this.m?this.m:String(this.m)};function ch(a){H.call(this,a)}
u(ch,H);function dh(a){H.call(this,a,-1,eh)}
u(dh,H);var eh=[1];var fh=["platform","platformVersion","architecture","model","uaFullVersion"];new dh;function gh(a){H.call(this,a)}
u(gh,H);function hh(a){H.call(this,a,31,ih)}
u(hh,H);var ih=[3,20,27];function jh(a){H.call(this,a,17,kh)}
u(jh,H);var kh=[3,5];function lh(a){H.call(this,a,6,mh)}
u(lh,H);var mh=[5];function nh(a){H.call(this,a)}
u(nh,H);var oh;oh=new function(a,b,c){this.i=a;this.fieldName=b;this.h=c;this.isRepeated=0;this.j=pe}(175237375,{tr:0},nh);function ph(a,b,c,d,e,f,g,h,k,m,q){ff.call(this);var r=this;this.O="";this.j=[];this.Mb="";this.Nb=this.sa=-1;this.Ya=!1;this.I=this.m=null;this.F=0;this.Dc=1;this.timeoutMillis=0;this.ca=!1;ff.call(this);this.Za=b||function(){};
this.s=new qh(a,f);this.Bc=d;this.Xa=q;this.Ec=Ya(lf,0,1);this.S=e||null;this.K=c||null;this.V=g||!1;this.pageId=k||null;this.logger=null;this.withCredentials=!h;this.Ha=f||!1;!this.Ha&&(65<=bc("Chromium")||45<=bc("Firefox")||12<=bc("Safari")||(Ec()||C("iPad")||C("iPod"))&&Fc());a=D(new gh,1,1);rh(this.s,a);this.o=new Cg(1E4);this.i=new Tf(this.o.getValue());we(this,this.i);m=sh(this,m);Ve(this.i,"tick",m,!1,this);this.P=new Tf(6E5);we(this,this.P);Ve(this.P,"tick",m,!1,this);this.V||this.P.start();
this.Ha||(Ve(document,"visibilitychange",function(){"hidden"===document.visibilityState&&r.W()}),Ve(document,"pagehide",this.W,!1,this))}
u(ph,ff);function sh(a,b){return b?function(){b().then(function(){a.flush()})}:function(){a.flush()}}
ph.prototype.C=function(){this.W();ff.prototype.C.call(this)};
function th(a){a.S||(a.S=.01>a.Ec()?"https://www.google.com/log?format=json&hasfast=true":"https://play.google.com/log?format=json&hasfast=true");return a.S}
function uh(a,b){a.o=new Cg(1>b?1:b);Uf(a.i,a.o.getValue())}
ph.prototype.log=function(a){a=a.clone();var b=this.Dc++;D(a,21,b);this.O&&D(a,26,this.O);if(!Td(a,1)){b=a;var c=Date.now().toString();D(b,1,c)}null!=Td(a,15,!1)||D(a,15,60*(new Date).getTimezoneOffset());this.m&&(b=this.m.clone(),G(a,Ag,16,b));for(;1E3<=this.j.length;)this.j.shift(),++this.F;this.j.push(a);gf(this,new vh(a));this.V||this.i.enabled||this.i.start()};
ph.prototype.flush=function(a,b){var c=this;if(0===this.j.length)a&&a();else if(this.ca)wh(this);else{var d=Date.now();if(this.Nb>d&&this.sa<d)b&&b("throttled");else{var e=xh(this.s,this.j,this.F);d={};var f=this.Za();f&&(d.Authorization=f);var g=th(this);this.K&&(d["X-Goog-AuthUser"]=this.K,g=vc(g,"authuser",this.K));this.pageId&&(d["X-Goog-PageId"]=this.pageId,g=vc(g,"pageId",this.pageId));if(f&&this.Mb===f)b&&b("stale-auth-token");else{this.j=[];this.i.enabled&&this.i.stop();this.F=0;var h=me(e),
k;this.I&&this.I.isSupported(h.length)&&(k=this.I.compress(h));var m={url:g,body:h,Ic:1,Gb:d,requestType:"POST",withCredentials:this.withCredentials,timeoutMillis:this.timeoutMillis},q=function(t){c.o.reset();Uf(c.i,c.o.getValue());if(t){var y=null;try{var E=JSON.parse(t.replace(")]}'\n",""));y=new lh(E)}catch(F){}y&&(t=Number(ge(Td(y,1),"-1")),0<t&&(c.sa=Date.now(),c.Nb=c.sa+t),y=oh.j(y))&&(y=ge(Td(y,1),-1),-1!=y&&(c.Ya||uh(c,y)))}a&&a()},r=function(t,y){var E=be(e,hh,3),F=c.o;
F.h=Math.min(3E5,2*F.h);F.i=Math.min(3E5,F.h+Math.round(.2*(Math.random()-.5)*F.h));Uf(c.i,c.o.getValue());401===t&&f&&(c.Mb=f);void 0===y&&(y=500<=t&&600>t||401===t||0===t);y&&(c.j=E.concat(c.j),c.V||c.i.enabled||c.i.start());b&&b("net-send-failed",t)},w=function(){c.Xa?c.Xa.send(m,q,r):c.Bc(m,q,r)};
k?k.then(function(t){m.Gb["Content-Encoding"]="gzip";m.Gb["Content-Type"]="application/binary";m.body=t;m.Ic=2;w()},function(){w()}):w()}}}};
ph.prototype.W=function(){this.flush()};
function wh(a){yh(a,function(b,c){b=vc(b,"format","json");b=window.navigator.sendBeacon(b,me(c));a.ca&&!b&&(a.ca=!1);return b})}
function yh(a,b){if(0!==a.j.length){var c=Cc(th(a),"format");c=tc(c,"auth",a.Za(),"authuser",a.K||"0");for(var d=0;10>d&&a.j.length;++d){var e=a.j.slice(0,32),f=xh(a.s,e,a.F);if(!b(c,f))break;a.F=0;a.j=a.j.slice(e.length)}a.i.enabled&&a.i.stop()}}
function vh(){ye.call(this,"event-logged",void 0)}
u(vh,ye);function qh(a,b){this.i=b=void 0===b?!1:b;this.uach=this.locale=null;this.h=new jh;D(this.h,2,a);b||(this.locale=document.documentElement.getAttribute("lang"));rh(this,new gh)}
function rh(a,b){G(a.h,gh,1,b);Td(b,1)||D(b,1,1);a.i||(b=zh(a),Td(b,5)||D(b,5,a.locale));a.uach&&(b=zh(a),Zd(b,dh,9)||G(b,dh,9,a.uach))}
function Ah(a,b){var c=void 0===c?fh:c;b(window,c).then(function(d){a.uach=d;d=zh(a);G(d,dh,9,a.uach);return!0}).catch(function(){return!1})}
function zh(a){a=Zd(a.h,gh,1);var b=Zd(a,ch,11);b||(b=new ch,G(a,ch,11,b));return b}
function xh(a,b,c){c=void 0===c?0:c;a=a.h.clone();var d=Date.now().toString();a=D(a,4,d);b=de(a,hh,3,b);c&&D(b,14,c);return b}
;function Bh(a,b,c){Ug(a.url,function(d){d=d.target;if(bh(d)){try{var e=d.A?d.A.responseText:""}catch(f){e=""}b(e)}else c(d.getStatus())},a.requestType,a.body,a.Gb,a.timeoutMillis,a.withCredentials)}
;function Ch(){this.j="https://play.google.com/log?format=json&hasfast=true";this.s=!1;this.m=Bh;this.h=""}
;function Dh(){var a=void 0===a?"":a;var b=void 0===b?"":b;var c=new Ch;c.h="";""!=a&&(c.j=a);b&&(c.i=b);a=new ph(1828,c.I?c.I:zg,"0",c.m,c.j,c.s,!1,c.P,void 0,void 0,c.o?c.o:void 0);c.M&&rh(a.s,c.M);if(c.i){b=c.i;var d=zh(a.s);D(d,7,b)}c.l&&(a.I=c.l);c.h&&(a.O=c.h);c.v&&((b=c.v)?(a.m||(a.m=new Ag),b=me(b),D(a.m,4,b)):a.m&&D(a.m,4,void 0,!1));c.K&&(d=c.K,a.m||(a.m=new Ag),b=a.m,d=null==d?Fd:Bd(d,1),D(b,2,d));c.F&&(b=c.F,a.Ya=!0,uh(a,b));c.O&&Ah(a.s,c.O);this.h=a}
Dh.prototype.flush=function(a){var b=a||[];if(b.length){a=new lg;for(var c=[],d=0;d<b.length;d++){var e=b[d],f=e,g=new dg;var h=D(g,1,f.i);var k=f;g=[];for(var m=0;m<k.h.length;m++)g.push(k.h[m].La);if(null==g)g=Fd;else{for(k=0;k<g.length;k++);g=Bd(g,5)}g=D(h,3,g);h=[];k=[];m=p(f.Aa.keys());for(var q=m.next();!q.done;q=m.next())k.push(q.value.split(","));for(m=0;m<k.length;m++){q=k[m];var r=f.l;for(var w=f.ub(q)||[],t=[],y=0;y<w.length;y++){var E=w[y];E=E&&E.Sb;var F=new ig;switch(r){case 3:Xd(F,
1,kg,Number(E));break;case 2:Xd(F,2,kg,Number(E))}t.push(F)}r=t;for(w=0;w<r.length;w++){t=r[w];y=new fg;t=G(y,ig,2,t);y=q;E=[];F=f;for(var O=[],N=0;N<F.h.length;N++)O.push(F.h[N].Ma);F=O;for(O=0;O<F.length;O++){N=F[O];var Q=y[O],da=new hg;switch(N){case 3:Xd(da,1,jg,String(Q));break;case 2:Xd(da,2,jg,Number(Q));break;case 1:Xd(da,3,jg,"true"==Q)}E.push(da)}de(t,hg,1,E);h.push(t)}}de(g,fg,4,h);c.push(g);e.clear()}de(a,dg,1,c);b=this.h;a instanceof hh?b.log(a):(c=new hh,a=me(a),a=D(c,8,a),b.log(a));
this.h.flush()}};function Eh(a){this.wa=a;this.v=Fh();this.l=new Dh;this.h=new Wf(this.l);this.o=new ag(this.h);this.i=new bg(this.h);this.j=new cg(this.h);this.m=new $f(this.h)}
function Fh(){var a,b,c;return null!=(c=null==(a=globalThis.performance)?void 0:null==(b=a.now)?void 0:b.call(a))?c:Date.now()}
;function Gh(){var a=this;this.promise=new Promise(function(b,c){a.resolve=b;a.reject=c})}
;function Hh(a){var b=this;this.i=!1;var c=a.program;var d=a.Tc;if(a.gd){var e;this.la=null!=(e=a.la)?e:new Eh(d)}var f=new Gh;this.j=f.promise;if(!z[d]){var g;null!=(g=this.la)&&g.m.h.rb("/client_streamz/bg/fiec",g.wa,1)}this.l=p((0,z[d].a)(c,function(h,k){Promise.resolve().then(function(){var m;if(null!=(m=b.la)){var q=Fh()-m.v;m.o.h.sb("/client_streamz/bg/fil",q,m.wa)}f.resolve({Gc:h,ud:k})})},!0)).next().value;
this.td=f.promise.then(function(){})}
Hh.prototype.snapshot=function(a){var b=this;if(this.i)throw Error("Already disposed");var c=Fh(),d;null!=(d=this.la)&&d.i.h.rb("/client_streamz/bg/fsc",d.wa);return this.j.then(function(e){var f=e.Gc;return new Promise(function(g){f(function(h){var k;if(null!=(k=b.la)){var m=Fh()-c;k.j.h.sb("/client_streamz/bg/fsl",m,k.wa)}g(h)},[a.Ub,
a.vd])})})};
Hh.prototype.tc=function(a){if(this.i)throw Error("Already disposed");var b=Fh(),c;null!=(c=this.la)&&c.i.h.rb("/client_streamz/bg/fsc",c.wa);a=this.l([a.Ub,a.vd]);null!=(c=this.la)&&(b=Fh()-b,c.j.h.sb("/client_streamz/bg/fsl",b,c.wa));return a};
Hh.prototype.dispose=function(){var a;null!=(a=this.la)&&a.h.tb();this.i=!0;this.j.then(function(b){(b=b.ud)&&b()})};
Hh.prototype.h=function(){return this.i};var Ih=window;Cb("csi.gstatic.com");Cb("googleads.g.doubleclick.net");Cb("partner.googleadservices.com");Cb("pubads.g.doubleclick.net");Cb("securepubads.g.doubleclick.net");Cb("tpc.googlesyndication.com");/*

 SPDX-License-Identifier: Apache-2.0
*/
var Jh;try{new URL("s://g"),Jh=!0}catch(a){Jh=!1}var Kh=Jh;function Lh(a,b){a.src=Hb(b);var c,d;(c=(b=null==(d=(c=(a.ownerDocument&&a.ownerDocument.defaultView||window).document).querySelector)?void 0:d.call(c,"script[nonce]"))?b.nonce||b.getAttribute("nonce")||"":"")&&a.setAttribute("nonce",c)}
;function Mh(a){this.Yc=a}
function Nh(a){return new Mh(function(b){return b.substr(0,a.length+1).toLowerCase()===a+":"})}
var Oh=[Nh("data"),Nh("http"),Nh("https"),Nh("mailto"),Nh("ftp"),new Mh(function(a){return/^[^:]*([/?#]|$)/.test(a)})];function Ph(a){var b=Qh;if(b)for(var c in b)Object.prototype.hasOwnProperty.call(b,c)&&a(b[c],c,b)}
function Rh(){var a=[];Ph(function(b){a.push(b)});
return a}
var Qh={Pd:"allow-forms",Qd:"allow-modals",Rd:"allow-orientation-lock",Sd:"allow-pointer-lock",Td:"allow-popups",Ud:"allow-popups-to-escape-sandbox",Vd:"allow-presentation",Wd:"allow-same-origin",Xd:"allow-scripts",Yd:"allow-top-navigation",Zd:"allow-top-navigation-by-user-activation"},Sh=db(function(){return Rh()});
function Th(){var a=Uh(),b={};fb(Sh(),function(c){a.sandbox&&a.sandbox.supports&&a.sandbox.supports(c)&&(b[c]=!0)});
return b}
function Uh(){var a=void 0===a?document:a;return a.createElement("iframe")}
;function Vh(a){"number"==typeof a&&(a=Math.round(a)+"px");return a}
;var Wh=(new Date).getTime();var Xh="client_dev_domain client_dev_regex_map client_dev_root_url client_rollout_override expflag forcedCapability jsfeat jsmode mods".split(" ");ia(Xh);function Yh(a){ff.call(this);var b=this;this.s=this.j=0;this.Y=null!=a?a:{R:function(e,f){return setTimeout(e,f)},
ea:function(e){clearTimeout(e)}};
var c,d;this.i=null!=(d=null==(c=window.navigator)?void 0:c.onLine)?d:!0;this.m=function(){return x(function(e){return v(e,Zh(b),0)})};
window.addEventListener("offline",this.m);window.addEventListener("online",this.m);this.s||$h(this)}
u(Yh,ff);function ai(){var a=bi;Yh.h||(Yh.h=new Yh(a));return Yh.h}
Yh.prototype.dispose=function(){window.removeEventListener("offline",this.m);window.removeEventListener("online",this.m);this.Y.ea(this.s);delete Yh.h};
Yh.prototype.U=function(){return this.i};
function $h(a){a.s=a.Y.R(function(){var b;return x(function(c){if(1==c.h)return a.i?(null==(b=window.navigator)?0:b.onLine)?c.u(3):v(c,Zh(a),3):v(c,Zh(a),3);$h(a);c.h=0})},3E4)}
function Zh(a,b){return a.o?a.o:a.o=new Promise(function(c){var d,e,f,g;return x(function(h){switch(h.h){case 1:return d=window.AbortController?new window.AbortController:void 0,f=null==(e=d)?void 0:e.signal,g=!1,ya(h,2,3),d&&(a.j=a.Y.R(function(){d.abort()},b||2E4)),v(h,fetch("/generate_204",{method:"HEAD",
signal:f}),5);case 5:g=!0;case 3:Ba(h);a.o=void 0;a.j&&(a.Y.ea(a.j),a.j=0);g!==a.i&&(a.i=g,a.i?gf(a,"networkstatus-online"):gf(a,"networkstatus-offline"));c(g);Ca(h);break;case 2:Aa(h),g=!1,h.u(3)}})})}
;function ci(){this.data_=[];this.h=-1}
ci.prototype.set=function(a,b){b=void 0===b?!0:b;0<=a&&52>a&&Number.isInteger(a)&&this.data_[a]!==b&&(this.data_[a]=b,this.h=-1)};
ci.prototype.get=function(a){return!!this.data_[a]};
function di(a){-1===a.h&&(a.h=ib(a.data_,function(b,c,d){return c?b+Math.pow(2,d):b},0));
return a.h}
;function ei(a,b){this.h=a[z.Symbol.iterator]();this.i=b}
ei.prototype[Symbol.iterator]=function(){return this};
ei.prototype.next=function(){var a=this.h.next();return{value:a.done?void 0:this.i.call(void 0,a.value),done:a.done}};
function fi(a,b){return new ei(a,b)}
;function gi(){this.blockSize=-1}
;function hi(){this.blockSize=-1;this.blockSize=64;this.h=[];this.m=[];this.o=[];this.j=[];this.j[0]=128;for(var a=1;a<this.blockSize;++a)this.j[a]=0;this.l=this.i=0;this.reset()}
Za(hi,gi);hi.prototype.reset=function(){this.h[0]=1732584193;this.h[1]=4023233417;this.h[2]=2562383102;this.h[3]=271733878;this.h[4]=3285377520;this.l=this.i=0};
function ii(a,b,c){c||(c=0);var d=a.o;if("string"===typeof b)for(var e=0;16>e;e++)d[e]=b.charCodeAt(c)<<24|b.charCodeAt(c+1)<<16|b.charCodeAt(c+2)<<8|b.charCodeAt(c+3),c+=4;else for(e=0;16>e;e++)d[e]=b[c]<<24|b[c+1]<<16|b[c+2]<<8|b[c+3],c+=4;for(e=16;80>e;e++){var f=d[e-3]^d[e-8]^d[e-14]^d[e-16];d[e]=(f<<1|f>>>31)&4294967295}b=a.h[0];c=a.h[1];var g=a.h[2],h=a.h[3],k=a.h[4];for(e=0;80>e;e++){if(40>e)if(20>e){f=h^c&(g^h);var m=1518500249}else f=c^g^h,m=1859775393;else 60>e?(f=c&g|h&(c|g),m=2400959708):
(f=c^g^h,m=3395469782);f=(b<<5|b>>>27)+f+k+m+d[e]&4294967295;k=h;h=g;g=(c<<30|c>>>2)&4294967295;c=b;b=f}a.h[0]=a.h[0]+b&4294967295;a.h[1]=a.h[1]+c&4294967295;a.h[2]=a.h[2]+g&4294967295;a.h[3]=a.h[3]+h&4294967295;a.h[4]=a.h[4]+k&4294967295}
hi.prototype.update=function(a,b){if(null!=a){void 0===b&&(b=a.length);for(var c=b-this.blockSize,d=0,e=this.m,f=this.i;d<b;){if(0==f)for(;d<=c;)ii(this,a,d),d+=this.blockSize;if("string"===typeof a)for(;d<b;){if(e[f]=a.charCodeAt(d),++f,++d,f==this.blockSize){ii(this,e);f=0;break}}else for(;d<b;)if(e[f]=a[d],++f,++d,f==this.blockSize){ii(this,e);f=0;break}}this.i=f;this.l+=b}};
hi.prototype.digest=function(){var a=[],b=8*this.l;56>this.i?this.update(this.j,56-this.i):this.update(this.j,this.blockSize-(this.i-56));for(var c=this.blockSize-1;56<=c;c--)this.m[c]=b&255,b/=256;ii(this,this.m);for(c=b=0;5>c;c++)for(var d=24;0<=d;d-=8)a[b]=this.h[c]>>d&255,++b;return a};function ji(a){return"string"==typeof a.className?a.className:a.getAttribute&&a.getAttribute("class")||""}
function ki(a,b){"string"==typeof a.className?a.className=b:a.setAttribute&&a.setAttribute("class",b)}
function li(a,b){a.classList?b=a.classList.contains(b):(a=a.classList?a.classList:ji(a).match(/\S+/g)||[],b=0<=eb(a,b));return b}
function mi(){var a=document.body;a.classList?a.classList.remove("inverted-hdpi"):li(a,"inverted-hdpi")&&ki(a,Array.prototype.filter.call(a.classList?a.classList:ji(a).match(/\S+/g)||[],function(b){return"inverted-hdpi"!=b}).join(" "))}
;function ni(){}
ni.prototype.next=function(){return oi};
var oi={done:!0,value:void 0};function pi(a){return{value:a,done:!1}}
ni.prototype.da=function(){return this};function qi(a){if(a instanceof ri||a instanceof si||a instanceof ti)return a;if("function"==typeof a.next)return new ri(function(){return a});
if("function"==typeof a[Symbol.iterator])return new ri(function(){return a[Symbol.iterator]()});
if("function"==typeof a.da)return new ri(function(){return a.da()});
throw Error("Not an iterator or iterable.");}
function ri(a){this.i=a}
ri.prototype.da=function(){return new si(this.i())};
ri.prototype[Symbol.iterator]=function(){return new ti(this.i())};
ri.prototype.h=function(){return new ti(this.i())};
function si(a){this.i=a}
u(si,ni);si.prototype.next=function(){return this.i.next()};
si.prototype[Symbol.iterator]=function(){return new ti(this.i)};
si.prototype.h=function(){return new ti(this.i)};
function ti(a){ri.call(this,function(){return a});
this.j=a}
u(ti,ri);ti.prototype.next=function(){return this.j.next()};function ui(a,b){this.i={};this.h=[];this.oa=this.size=0;var c=arguments.length;if(1<c){if(c%2)throw Error("Uneven number of arguments");for(var d=0;d<c;d+=2)this.set(arguments[d],arguments[d+1])}else if(a)if(a instanceof ui)for(c=a.xb(),d=0;d<c.length;d++)this.set(c[d],a.get(c[d]));else for(d in a)this.set(d,a[d])}
l=ui.prototype;l.xb=function(){vi(this);return this.h.concat()};
l.has=function(a){return wi(this.i,a)};
l.equals=function(a,b){if(this===a)return!0;if(this.size!=a.size)return!1;b=b||xi;vi(this);for(var c,d=0;c=this.h[d];d++)if(!b(this.get(c),a.get(c)))return!1;return!0};
function xi(a,b){return a===b}
l.Oa=function(){return 0==this.size};
l.clear=function(){this.i={};this.oa=this.size=this.h.length=0};
l.remove=function(a){return this.delete(a)};
l.delete=function(a){return wi(this.i,a)?(delete this.i[a],--this.size,this.oa++,this.h.length>2*this.size&&vi(this),!0):!1};
function vi(a){if(a.size!=a.h.length){for(var b=0,c=0;b<a.h.length;){var d=a.h[b];wi(a.i,d)&&(a.h[c++]=d);b++}a.h.length=c}if(a.size!=a.h.length){var e={};for(c=b=0;b<a.h.length;)d=a.h[b],wi(e,d)||(a.h[c++]=d,e[d]=1),b++;a.h.length=c}}
l.get=function(a,b){return wi(this.i,a)?this.i[a]:b};
l.set=function(a,b){wi(this.i,a)||(this.size+=1,this.h.push(a),this.oa++);this.i[a]=b};
l.forEach=function(a,b){for(var c=this.xb(),d=0;d<c.length;d++){var e=c[d],f=this.get(e);a.call(b,f,e,this)}};
l.clone=function(){return new ui(this)};
l.keys=function(){return qi(this.da(!0)).h()};
l.values=function(){return qi(this.da(!1)).h()};
l.entries=function(){var a=this;return fi(this.keys(),function(b){return[b,a.get(b)]})};
l.da=function(a){vi(this);var b=0,c=this.oa,d=this,e=new ni;e.next=function(){if(c!=d.oa)throw Error("The map has changed since the iterator was created");if(b>=d.h.length)return oi;var f=d.h[b++];return pi(a?f:d.i[f])};
return e};
function wi(a,b){return Object.prototype.hasOwnProperty.call(a,b)}
;function K(a){J.call(this);this.o=1;this.l=[];this.m=0;this.i=[];this.j={};this.s=!!a}
Za(K,J);l=K.prototype;l.subscribe=function(a,b,c){var d=this.j[a];d||(d=this.j[a]=[]);var e=this.o;this.i[e]=a;this.i[e+1]=b;this.i[e+2]=c;this.o=e+3;d.push(e);return e};
function yi(a,b,c,d){if(b=a.j[b]){var e=a.i;(b=b.find(function(f){return e[f+1]==c&&e[f+2]==d}))&&a.Ga(b)}}
l.Ga=function(a){var b=this.i[a];if(b){var c=this.j[b];0!=this.m?(this.l.push(a),this.i[a+1]=function(){}):(c&&kb(c,a),delete this.i[a],delete this.i[a+1],delete this.i[a+2])}return!!b};
l.ra=function(a,b){var c=this.j[a];if(c){for(var d=Array(arguments.length-1),e=1,f=arguments.length;e<f;e++)d[e-1]=arguments[e];if(this.s)for(e=0;e<c.length;e++){var g=c[e];zi(this.i[g+1],this.i[g+2],d)}else{this.m++;try{for(e=0,f=c.length;e<f&&!this.h();e++)g=c[e],this.i[g+1].apply(this.i[g+2],d)}finally{if(this.m--,0<this.l.length&&0==this.m)for(;c=this.l.pop();)this.Ga(c)}}return 0!=e}return!1};
function zi(a,b,c){zf(function(){a.apply(b,c)})}
l.clear=function(a){if(a){var b=this.j[a];b&&(b.forEach(this.Ga,this),delete this.j[a])}else this.i.length=0,this.j={}};
l.C=function(){K.X.C.call(this);this.clear();this.l.length=0};function Ai(a){this.h=a}
Ai.prototype.set=function(a,b){void 0===b?this.h.remove(a):this.h.set(a,Dg(b))};
Ai.prototype.get=function(a){try{var b=this.h.get(a)}catch(c){return}if(null!==b)try{return JSON.parse(b)}catch(c){throw"Storage: Invalid value was encountered";}};
Ai.prototype.remove=function(a){this.h.remove(a)};function Bi(a){this.h=a}
Za(Bi,Ai);function Ci(a){this.data=a}
function Di(a){return void 0===a||a instanceof Ci?a:new Ci(a)}
Bi.prototype.set=function(a,b){Bi.X.set.call(this,a,Di(b))};
Bi.prototype.i=function(a){a=Bi.X.get.call(this,a);if(void 0===a||a instanceof Object)return a;throw"Storage: Invalid value was encountered";};
Bi.prototype.get=function(a){if(a=this.i(a)){if(a=a.data,void 0===a)throw"Storage: Invalid value was encountered";}else a=void 0;return a};function Ei(a){this.h=a}
Za(Ei,Bi);Ei.prototype.set=function(a,b,c){if(b=Di(b)){if(c){if(c<Date.now()){Ei.prototype.remove.call(this,a);return}b.expiration=c}b.creation=Date.now()}Ei.X.set.call(this,a,b)};
Ei.prototype.i=function(a){var b=Ei.X.i.call(this,a);if(b){var c=b.creation,d=b.expiration;if(d&&d<Date.now()||c&&c>Date.now())Ei.prototype.remove.call(this,a);else return b}};function Fi(){}
;function Gi(){}
Za(Gi,Fi);Gi.prototype[Symbol.iterator]=function(){return qi(this.da(!0)).h()};
Gi.prototype.clear=function(){var a=Array.from(this);a=p(a);for(var b=a.next();!b.done;b=a.next())this.remove(b.value)};function Hi(a){this.h=a}
Za(Hi,Gi);l=Hi.prototype;l.isAvailable=function(){if(!this.h)return!1;try{return this.h.setItem("__sak","1"),this.h.removeItem("__sak"),!0}catch(a){return!1}};
l.set=function(a,b){try{this.h.setItem(a,b)}catch(c){if(0==this.h.length)throw"Storage mechanism: Storage disabled";throw"Storage mechanism: Quota exceeded";}};
l.get=function(a){a=this.h.getItem(a);if("string"!==typeof a&&null!==a)throw"Storage mechanism: Invalid value was encountered";return a};
l.remove=function(a){this.h.removeItem(a)};
l.da=function(a){var b=0,c=this.h,d=new ni;d.next=function(){if(b>=c.length)return oi;var e=c.key(b++);if(a)return pi(e);e=c.getItem(e);if("string"!==typeof e)throw"Storage mechanism: Invalid value was encountered";return pi(e)};
return d};
l.clear=function(){this.h.clear()};
l.key=function(a){return this.h.key(a)};function Ii(){var a=null;try{a=window.localStorage||null}catch(b){}this.h=a}
Za(Ii,Hi);function Ji(a,b){this.i=a;this.h=null;var c;if(c=Ic)c=!(9<=Number(Vc));if(c){Ki||(Ki=new ui);this.h=Ki.get(a);this.h||(b?this.h=document.getElementById(b):(this.h=document.createElement("userdata"),this.h.addBehavior("#default#userData"),document.body.appendChild(this.h)),Ki.set(a,this.h));try{this.h.load(this.i)}catch(d){this.h=null}}}
Za(Ji,Gi);var Li={".":".2E","!":".21","~":".7E","*":".2A","'":".27","(":".28",")":".29","%":"."},Ki=null;function Mi(a){return"_"+encodeURIComponent(a).replace(/[.!~*'()%]/g,function(b){return Li[b]})}
l=Ji.prototype;l.isAvailable=function(){return!!this.h};
l.set=function(a,b){this.h.setAttribute(Mi(a),b);Ni(this)};
l.get=function(a){a=this.h.getAttribute(Mi(a));if("string"!==typeof a&&null!==a)throw"Storage mechanism: Invalid value was encountered";return a};
l.remove=function(a){this.h.removeAttribute(Mi(a));Ni(this)};
l.da=function(a){var b=0,c=this.h.XMLDocument.documentElement.attributes,d=new ni;d.next=function(){if(b>=c.length)return oi;var e=c[b++];if(a)return pi(decodeURIComponent(e.nodeName.replace(/\./g,"%")).slice(1));e=e.nodeValue;if("string"!==typeof e)throw"Storage mechanism: Invalid value was encountered";return pi(e)};
return d};
l.clear=function(){for(var a=this.h.XMLDocument.documentElement,b=a.attributes.length;0<b;b--)a.removeAttribute(a.attributes[b-1].nodeName);Ni(this)};
function Ni(a){try{a.h.save(a.i)}catch(b){throw"Storage mechanism: Quota exceeded";}}
;function Oi(a,b){this.i=a;this.h=b+"::"}
Za(Oi,Gi);Oi.prototype.set=function(a,b){this.i.set(this.h+a,b)};
Oi.prototype.get=function(a){return this.i.get(this.h+a)};
Oi.prototype.remove=function(a){this.i.remove(this.h+a)};
Oi.prototype.da=function(a){var b=this.i[Symbol.iterator](),c=this,d=new ni;d.next=function(){var e=b.next();if(e.done)return e;for(e=e.value;e.slice(0,c.h.length)!=c.h;){e=b.next();if(e.done)return e;e=e.value}return pi(a?e.slice(c.h.length):c.i.get(e))};
return d};function Pi(a){this.name=a}
;var Qi=new Pi("rawColdConfigGroup");var Ri=new Pi("rawHotConfigGroup");function Si(a){H.call(this,a)}
u(Si,H);function Ti(a){H.call(this,a)}
u(Ti,H);Ti.prototype.getKey=function(){return Td(this,1)};
Ti.prototype.getValue=function(){return fe(this,2===Yd(this,Ui)?2:-1)};
var Ui=[2,3,4,5,6];function Vi(a){H.call(this,a)}
u(Vi,H);function Wi(a){H.call(this,a)}
u(Wi,H);function Xi(a){H.call(this,a,-1,Yi)}
u(Xi,H);var Yi=[2];function Zi(a){H.call(this,a,-1,$i)}
u(Zi,H);Zi.prototype.getPlayerType=function(){return Td(this,36)};
Zi.prototype.setHomeGroupInfo=function(a){return G(this,Xi,81,a)};
Zi.prototype.clearLocationPlayabilityToken=function(){return D(this,89,void 0,!1)};
var $i=[9,66,24,32,86,100,101];function aj(a){H.call(this,a,-1,bj)}
u(aj,H);var bj=[15,26,28];function cj(a){H.call(this,a,-1,dj)}
u(cj,H);var dj=[5];function ej(a){H.call(this,a)}
u(ej,H);function fj(a){H.call(this,a,-1,gj)}
u(fj,H);fj.prototype.setSafetyMode=function(a){return D(this,5,a)};
var gj=[12];function hj(a){H.call(this,a,-1,ij)}
u(hj,H);hj.prototype.m=function(a){G(this,Zi,1,a)};
var ij=[12];var jj=new Pi("continuationCommand");var kj=new Pi("webCommandMetadata");var lj=new Pi("signalServiceEndpoint");var mj={oi:"EMBEDDED_PLAYER_MODE_UNKNOWN",li:"EMBEDDED_PLAYER_MODE_DEFAULT",ni:"EMBEDDED_PLAYER_MODE_PFP",mi:"EMBEDDED_PLAYER_MODE_PFL"};var nj=new Pi("feedbackEndpoint");var oj={Mq:"WEB_DISPLAY_MODE_UNKNOWN",Iq:"WEB_DISPLAY_MODE_BROWSER",Kq:"WEB_DISPLAY_MODE_MINIMAL_UI",Lq:"WEB_DISPLAY_MODE_STANDALONE",Jq:"WEB_DISPLAY_MODE_FULLSCREEN"};function pj(a){H.call(this,a,-1,qj)}
u(pj,H);function rj(a){H.call(this,a)}
u(rj,H);rj.prototype.getKey=function(){return ge(Td(this,1),"")};
rj.prototype.getValue=function(){return ge(Td(this,2),"")};
var qj=[4,5];function sj(a){H.call(this,a)}
u(sj,H);function tj(a){H.call(this,a)}
u(tj,H);var uj=[2,3,4];function vj(a){H.call(this,a)}
u(vj,H);vj.prototype.getMessage=function(){return ge(Td(this,1),"")};function wj(a){H.call(this,a)}
u(wj,H);function xj(a){H.call(this,a)}
u(xj,H);function yj(a){H.call(this,a,-1,zj)}
u(yj,H);var zj=[10,17];function Aj(a){H.call(this,a)}
u(Aj,H);function Bj(a){H.call(this,a)}
u(Bj,H);function Cj(a){H.call(this,a)}
u(Cj,H);function Dj(a){H.call(this,a)}
u(Dj,H);function Ej(a){H.call(this,a)}
u(Ej,H);function Fj(a,b){G(a,Cj,1,b)}
Ej.prototype.i=function(a){D(this,2,a)};
function Gj(a){H.call(this,a)}
u(Gj,H);function Hj(a,b){G(a,Cj,1,b)}
;function Ij(a){H.call(this,a,-1,Jj)}
u(Ij,H);Ij.prototype.i=function(a){D(this,1,a)};
function Kj(a,b){G(a,Cj,2,b)}
var Jj=[3];function Lj(a){H.call(this,a)}
u(Lj,H);Lj.prototype.i=function(a){D(this,1,a)};function Mj(a){H.call(this,a)}
u(Mj,H);Mj.prototype.i=function(a){D(this,1,a)};function Nj(a){H.call(this,a)}
u(Nj,H);Nj.prototype.i=function(a){D(this,1,a)};function Oj(a){H.call(this,a)}
u(Oj,H);Oj.prototype.i=function(a){D(this,1,a)};function Pj(a){H.call(this,a)}
u(Pj,H);function Qj(a){H.call(this,a)}
u(Qj,H);function Rj(a){H.call(this,a,-1,Sj)}
u(Rj,H);Rj.prototype.getPlayerType=function(){var a=Td(this,7);return null==a?0:a};
Rj.prototype.setVideoId=function(a){return D(this,19,a)};
function Tj(a,b){ee(a,68,Uj,b)}
function Uj(a){H.call(this,a)}
u(Uj,H);Uj.prototype.getId=function(){return ge(Td(this,2),"")};
var Sj=[83,68];function Vj(a){H.call(this,a)}
u(Vj,H);function Wj(a){H.call(this,a)}
u(Wj,H);function Xj(a){H.call(this,a)}
u(Xj,H);function Yj(a){H.call(this,a,449)}
u(Yj,H);
var Zj=[23,24,11,6,7,5,2,3,13,20,21,22,28,32,37,229,241,45,59,225,288,72,73,78,208,156,202,215,74,76,79,80,111,85,91,97,100,102,105,119,126,127,136,146,148,151,157,158,159,163,164,168,444,176,222,383,177,178,179,411,184,188,189,190,191,193,194,195,196,197,198,199,200,201,402,320,203,204,205,206,258,259,260,261,327,209,219,226,227,232,233,234,240,244,247,248,249,251,256,257,266,254,255,270,272,278,291,293,300,304,308,309,310,311,313,314,319,321,323,324,328,330,331,332,334,337,338,340,344,348,350,351,
352,353,354,355,356,357,358,361,363,364,368,369,370,373,374,375,378,380,381,388,389,403,410,412,429,413,414,415,416,417,418,430,423,424,425,426,427,431,117,439,441,448];var ak={qj:0,bj:1,hj:2,ij:4,nj:8,jj:16,kj:32,pj:64,oj:128,dj:256,fj:512,mj:1024,ej:2048,gj:4096,cj:8192,lj:16384};function bk(a){H.call(this,a)}
u(bk,H);function ck(a){H.call(this,a)}
u(ck,H);ck.prototype.setVideoId=function(a){return Xd(this,1,xk,a)};
ck.prototype.getPlaylistId=function(){return fe(this,2===Yd(this,xk)?2:-1)};
var xk=[1,2];function yk(a){H.call(this,a,-1,zk)}
u(yk,H);var zk=[3];var Ak=new Pi("webPlayerShareEntityServiceEndpoint");var Bk=new Pi("playlistEditEndpoint");var Ck=new Pi("modifyChannelNotificationPreferenceEndpoint");var Dk=new Pi("unsubscribeEndpoint");var Ek=new Pi("subscribeEndpoint");function Fk(a,b){1<b.length?a[b[0]]=b[1]:1===b.length&&Object.assign(a,b[0])}
;var Gk=z.window,Hk,Ik,Jk=(null==Gk?void 0:null==(Hk=Gk.yt)?void 0:Hk.config_)||(null==Gk?void 0:null==(Ik=Gk.ytcfg)?void 0:Ik.data_)||{};A("yt.config_",Jk);function Kk(){Fk(Jk,arguments)}
function L(a,b){return a in Jk?Jk[a]:b}
function Lk(){var a=Jk.EXPERIMENT_FLAGS;return a?a.web_disable_gel_stp_ecatcher_killswitch:void 0}
;function M(a){a=Mk(a);return"string"===typeof a&&"false"===a?!1:!!a}
function Nk(a,b){a=Mk(a);return void 0===a&&void 0!==b?b:Number(a||0)}
function Ok(){return L("EXPERIMENTS_TOKEN","")}
function Mk(a){var b=L("EXPERIMENTS_FORCED_FLAGS",{})||{};return void 0!==b[a]?b[a]:L("EXPERIMENT_FLAGS",{})[a]}
function Pk(){for(var a=[],b=L("EXPERIMENTS_FORCED_FLAGS",{}),c=p(Object.keys(b)),d=c.next();!d.done;d=c.next())d=d.value,a.push({key:d,value:String(b[d])});c=L("EXPERIMENT_FLAGS",{});var e=p(Object.keys(c));for(d=e.next();!d.done;d=e.next())d=d.value,d.startsWith("force_")&&void 0===b[d]&&a.push({key:d,value:String(c[d])});return a}
;var Qk=[];function Rk(a){Qk.forEach(function(b){return b(a)})}
function Sk(a){return a&&window.yterr?function(){try{return a.apply(this,arguments)}catch(b){Tk(b)}}:a}
function Tk(a,b,c,d){var e=B("yt.logging.errors.log");e?e(a,"ERROR",b,c,d):(e=L("ERRORS",[]),e.push([a,"ERROR",b,c,d]),Kk("ERRORS",e));Rk(a)}
function Uk(a,b,c,d){var e=B("yt.logging.errors.log");e?e(a,"WARNING",b,c,d):(e=L("ERRORS",[]),e.push([a,"WARNING",b,c,d]),Kk("ERRORS",e))}
;function Vk(){var a=Wk;B("yt.ads.biscotti.getId_")||A("yt.ads.biscotti.getId_",a)}
function Xk(a){A("yt.ads.biscotti.lastId_",a)}
;var Yk=/^[\w.]*$/,Zk={q:!0,search_query:!0};function $k(a,b){b=a.split(b);for(var c={},d=0,e=b.length;d<e;d++){var f=b[d].split("=");if(1==f.length&&f[0]||2==f.length)try{var g=al(f[0]||""),h=al(f[1]||"");g in c?Array.isArray(c[g])?lb(c[g],h):c[g]=[c[g],h]:c[g]=h}catch(r){var k=r,m=f[0],q=String($k);k.args=[{key:m,value:f[1],query:a,method:bl==q?"unchanged":q}];Zk.hasOwnProperty(m)||Uk(k)}}return c}
var bl=String($k);function cl(a){var b=[];mb(a,function(c,d){var e=encodeURIComponent(String(d)),f;Array.isArray(c)?f=c:f=[c];fb(f,function(g){""==g?b.push(e):b.push(e+"="+encodeURIComponent(String(g)))})});
return b.join("&")}
function dl(a){"?"==a.charAt(0)&&(a=a.substr(1));return $k(a,"&")}
function el(a){return-1!=a.indexOf("?")?(a=(a||"").split("#")[0],a=a.split("?",2),dl(1<a.length?a[1]:a[0])):{}}
function fl(a,b,c){var d=a.split("#",2);a=d[0];d=1<d.length?"#"+d[1]:"";var e=a.split("?",2);a=e[0];e=dl(e[1]||"");for(var f in b)!c&&null!==e&&f in e||(e[f]=b[f]);return uc(a,e)+d}
function gl(a){if(!b)var b=window.location.href;var c=mc(1,a),d=nc(a);c&&d?(a=a.match(kc),b=b.match(kc),a=a[3]==b[3]&&a[1]==b[1]&&a[4]==b[4]):a=d?nc(b)==d&&(Number(mc(4,b))||null)==(Number(mc(4,a))||null):!0;return a}
function al(a){return a&&a.match(Yk)?a:decodeURIComponent(a.replace(/\+/g," "))}
;function hl(a){var b=il;a=void 0===a?B("yt.ads.biscotti.lastId_")||"":a;var c=Object,d=c.assign,e={};e.dt=Wh;e.flash="0";a:{try{var f=b.h.top.location.href}catch(fa){f=2;break a}f=f?f===b.i.location.href?0:1:2}e=(e.frm=f,e);try{e.u_tz=-(new Date).getTimezoneOffset();var g=void 0===g?Ih:g;try{var h=g.history.length}catch(fa){h=0}e.u_his=h;var k;e.u_h=null==(k=Ih.screen)?void 0:k.height;var m;e.u_w=null==(m=Ih.screen)?void 0:m.width;var q;e.u_ah=null==(q=Ih.screen)?void 0:q.availHeight;var r;e.u_aw=
null==(r=Ih.screen)?void 0:r.availWidth;var w;e.u_cd=null==(w=Ih.screen)?void 0:w.colorDepth}catch(fa){}h=b.h;try{var t=h.screenX;var y=h.screenY}catch(fa){}try{var E=h.outerWidth;var F=h.outerHeight}catch(fa){}try{var O=h.innerWidth;var N=h.innerHeight}catch(fa){}try{var Q=h.screenLeft;var da=h.screenTop}catch(fa){}try{O=h.innerWidth,N=h.innerHeight}catch(fa){}try{var U=h.screen.availWidth;var qb=h.screen.availTop}catch(fa){}t=[Q,da,t,y,U,qb,E,F,O,N];try{var Wa=(b.h.top||window).document,oa="CSS1Compat"==
Wa.compatMode?Wa.documentElement:Wa.body;var I=(new nf(oa.clientWidth,oa.clientHeight)).round()}catch(fa){I=new nf(-12245933,-12245933)}Wa=I;I={};var ma=void 0===ma?z:ma;oa=new ci;ma.SVGElement&&ma.document.createElementNS&&oa.set(0);y=Th();y["allow-top-navigation-by-user-activation"]&&oa.set(1);y["allow-popups-to-escape-sandbox"]&&oa.set(2);ma.crypto&&ma.crypto.subtle&&oa.set(3);ma.TextDecoder&&ma.TextEncoder&&oa.set(4);ma=di(oa);I.bc=ma;I.bih=Wa.height;I.biw=Wa.width;I.brdim=t.join();b=b.i;b=(I.vis=
b.prerendering?3:{visible:1,hidden:2,prerender:3,preview:4,unloaded:5}[b.visibilityState||b.webkitVisibilityState||b.mozVisibilityState||""]||0,I.wgl=!!Ih.WebGLRenderingContext,I);c=d.call(c,e,b);c.ca_type="image";a&&(c.bid=a);return c}
var il=new function(){var a=window.document;this.h=window;this.i=a};
A("yt.ads_.signals_.getAdSignalsString",function(a){return cl(hl(a))});Date.now();var jl="XMLHttpRequest"in z?function(){return new XMLHttpRequest}:null;
function kl(){if(!jl)return null;var a=jl();return"open"in a?a:null}
function ll(a){switch(a&&"status"in a?a.status:-1){case 200:case 201:case 202:case 203:case 204:case 205:case 206:case 304:return!0;default:return!1}}
;function ml(a,b){"function"===typeof a&&(a=Sk(a));return window.setTimeout(a,b)}
;var nl={Authorization:"AUTHORIZATION","X-Goog-EOM-Visitor-Id":"EOM_VISITOR_DATA","X-Goog-Visitor-Id":"SANDBOXED_VISITOR_ID","X-Youtube-Domain-Admin-State":"DOMAIN_ADMIN_STATE","X-Youtube-Chrome-Connected":"CHROME_CONNECTED_HEADER","X-YouTube-Client-Name":"INNERTUBE_CONTEXT_CLIENT_NAME","X-YouTube-Client-Version":"INNERTUBE_CONTEXT_CLIENT_VERSION","X-YouTube-Delegation-Context":"INNERTUBE_CONTEXT_SERIALIZED_DELEGATION_CONTEXT","X-YouTube-Device":"DEVICE","X-Youtube-Identity-Token":"ID_TOKEN","X-YouTube-Page-CL":"PAGE_CL",
"X-YouTube-Page-Label":"PAGE_BUILD_LABEL","X-YouTube-Variants-Checksum":"VARIANTS_CHECKSUM"},ol="app debugcss debugjs expflag force_ad_params force_ad_encrypted force_viral_ad_response_params forced_experiments innertube_snapshots innertube_goldens internalcountrycode internalipoverride absolute_experiments conditional_experiments sbb sr_bns_address".split(" ").concat(ia(Xh)),pl=!1;
function ql(a,b){b=void 0===b?{}:b;var c=gl(a),d=M("web_ajax_ignore_global_headers_if_set"),e;for(e in nl){var f=L(nl[e]);"X-Goog-Visitor-Id"!==e||f||(f=L("VISITOR_DATA"));!f||!c&&nc(a)||d&&void 0!==b[e]||(b[e]=f)}"X-Goog-EOM-Visitor-Id"in b&&"X-Goog-Visitor-Id"in b&&delete b["X-Goog-Visitor-Id"];if(c||!nc(a))b["X-YouTube-Utc-Offset"]=String(-(new Date).getTimezoneOffset());if(c||!nc(a)){try{var g=(new Intl.DateTimeFormat).resolvedOptions().timeZone}catch(h){}g&&(b["X-YouTube-Time-Zone"]=g)}document.location.hostname.endsWith("youtubeeducation.com")||
!c&&nc(a)||(b["X-YouTube-Ad-Signals"]=cl(hl()));return b}
function rl(a){var b=window.location.search,c=nc(a);M("debug_handle_relative_url_for_query_forward_killswitch")||!c&&gl(a)&&(c=document.location.hostname);var d=lc(mc(5,a));d=(c=c&&(c.endsWith("youtube.com")||c.endsWith("youtube-nocookie.com")))&&d&&d.startsWith("/api/");if(!c||d)return a;var e=dl(b),f={};fb(ol,function(g){e[g]&&(f[g]=e[g])});
return fl(a,f||{},!1)}
function sl(a,b){var c=b.format||"JSON";a=tl(a,b);var d=ul(a,b),e=!1,f=vl(a,function(k){if(!e){e=!0;h&&window.clearTimeout(h);var m=ll(k),q=null,r=400<=k.status&&500>k.status,w=500<=k.status&&600>k.status;if(m||r||w)q=wl(a,c,k,b.convertToSafeHtml);if(m)a:if(k&&204==k.status)m=!0;else{switch(c){case "XML":m=0==parseInt(q&&q.return_code,10);break a;case "RAW":m=!0;break a}m=!!q}q=q||{};r=b.context||z;m?b.onSuccess&&b.onSuccess.call(r,k,q):b.onError&&b.onError.call(r,k,q);b.onFinish&&b.onFinish.call(r,
k,q)}},b.method,d,b.headers,b.responseType,b.withCredentials);
d=b.timeout||0;if(b.onTimeout&&0<d){var g=b.onTimeout;var h=ml(function(){e||(e=!0,f.abort(),window.clearTimeout(h),g.call(b.context||z,f))},d)}return f}
function tl(a,b){b.includeDomain&&(a=document.location.protocol+"//"+document.location.hostname+(document.location.port?":"+document.location.port:"")+a);var c=L("XSRF_FIELD_NAME");if(b=b.urlParams)b[c]&&delete b[c],a=fl(a,b||{},!0);return a}
function ul(a,b){var c=L("XSRF_FIELD_NAME"),d=L("XSRF_TOKEN"),e=b.postBody||"",f=b.postParams,g=L("XSRF_FIELD_NAME"),h;b.headers&&(h=b.headers["Content-Type"]);b.excludeXsrf||nc(a)&&!b.withCredentials&&nc(a)!=document.location.hostname||"POST"!=b.method||h&&"application/x-www-form-urlencoded"!=h||b.postParams&&b.postParams[g]||(f||(f={}),f[c]=d);(M("ajax_parse_query_data_only_when_filled")&&f&&0<Object.keys(f).length||f)&&"string"===typeof e&&(e=dl(e),xb(e,f),e=b.postBodyFormat&&"JSON"==b.postBodyFormat?
JSON.stringify(e):sc(e));f=e||f&&!pb(f);!pl&&f&&"POST"!=b.method&&(pl=!0,Tk(Error("AJAX request with postData should use POST")));return e}
function wl(a,b,c,d){var e=null;switch(b){case "JSON":try{var f=c.responseText}catch(g){throw d=Error("Error reading responseText"),d.params=a,Uk(d),g;}a=c.getResponseHeader("Content-Type")||"";f&&0<=a.indexOf("json")&&(")]}'\n"===f.substring(0,5)&&(f=f.substring(5)),e=JSON.parse(f));break;case "XML":if(a=(a=c.responseXML)?xl(a):null)e={},fb(a.getElementsByTagName("*"),function(g){e[g.tagName]=yl(g)})}d&&zl(e);
return e}
function zl(a){if(Qa(a))for(var b in a){var c;(c="html_content"==b)||(c=b.length-5,c=0<=c&&b.indexOf("_html",c)==c);if(c){c=b;Cb("HTML that is escaped and sanitized server-side and passed through yt.net.ajax");var d=a[b],e=zb();d=e?e.createHTML(d):d;a[c]=new dc(d)}else zl(a[b])}}
function xl(a){return a?(a=("responseXML"in a?a.responseXML:a).getElementsByTagName("root"))&&0<a.length?a[0]:null:null}
function yl(a){var b="";fb(a.childNodes,function(c){b+=c.nodeValue});
return b}
function vl(a,b,c,d,e,f,g){function h(){4==(k&&"readyState"in k?k.readyState:0)&&b&&Sk(b)(k)}
c=void 0===c?"GET":c;d=void 0===d?"":d;var k=kl();if(!k)return null;"onloadend"in k?k.addEventListener("loadend",h,!1):k.onreadystatechange=h;M("debug_forward_web_query_parameters")&&(a=rl(a));k.open(c,a,!0);f&&(k.responseType=f);g&&(k.withCredentials=!0);c="POST"==c&&(void 0===window.FormData||!(d instanceof FormData));if(e=ql(a,e))for(var m in e)k.setRequestHeader(m,e[m]),"content-type"==m.toLowerCase()&&(c=!1);c&&k.setRequestHeader("Content-Type","application/x-www-form-urlencoded");k.send(d);
return k}
;function Al(a){var b=this;this.i=void 0;this.h=!1;a.addEventListener("beforeinstallprompt",function(c){c.preventDefault();b.i=c});
a.addEventListener("appinstalled",function(){b.h=!0},{once:!0})}
function Bl(){if(!z.matchMedia)return"WEB_DISPLAY_MODE_UNKNOWN";try{return z.matchMedia("(display-mode: standalone)").matches?"WEB_DISPLAY_MODE_STANDALONE":z.matchMedia("(display-mode: minimal-ui)").matches?"WEB_DISPLAY_MODE_MINIMAL_UI":z.matchMedia("(display-mode: fullscreen)").matches?"WEB_DISPLAY_MODE_FULLSCREEN":z.matchMedia("(display-mode: browser)").matches?"WEB_DISPLAY_MODE_BROWSER":"WEB_DISPLAY_MODE_UNKNOWN"}catch(a){return"WEB_DISPLAY_MODE_UNKNOWN"}}
;function Cl(a,b,c,d,e){vg.set(""+a,b,{ib:c,path:"/",domain:void 0===d?"youtube.com":d,secure:void 0===e?!1:e})}
function Dl(a,b,c){vg.remove(""+a,void 0===b?"/":b,void 0===c?"youtube.com":c)}
function El(){if(!vg.isEnabled())return!1;if(!vg.Oa())return!0;vg.set("TESTCOOKIESENABLED","1",{ib:60});if("1"!==vg.get("TESTCOOKIESENABLED"))return!1;vg.remove("TESTCOOKIESENABLED");return!0}
;var Fl=B("ytglobal.prefsUserPrefsPrefs_")||{};A("ytglobal.prefsUserPrefsPrefs_",Fl);function Gl(){this.h=L("ALT_PREF_COOKIE_NAME","PREF");this.i=L("ALT_PREF_COOKIE_DOMAIN","youtube.com");var a=vg.get(""+this.h,void 0);if(a){a=decodeURIComponent(a).split("&");for(var b=0;b<a.length;b++){var c=a[b].split("="),d=c[0];(c=c[1])&&(Fl[d]=c.toString())}}}
Gl.prototype.get=function(a,b){Hl(a);Il(a);a=void 0!==Fl[a]?Fl[a].toString():null;return null!=a?a:b?b:""};
Gl.prototype.set=function(a,b){Hl(a);Il(a);if(null==b)throw Error("ExpectedNotNull");Fl[a]=b.toString()};
function Jl(a){return!!((Kl("f"+(Math.floor(a/31)+1))||0)&1<<a%31)}
Gl.prototype.remove=function(a){Hl(a);Il(a);delete Fl[a]};
Gl.prototype.clear=function(){for(var a in Fl)delete Fl[a]};
function Il(a){if(/^f([1-9][0-9]*)$/.test(a))throw Error("ExpectedRegexMatch: "+a);}
function Hl(a){if(!/^\w+$/.test(a))throw Error("ExpectedRegexMismatch: "+a);}
function Kl(a){a=void 0!==Fl[a]?Fl[a].toString():null;return null!=a&&/^[A-Fa-f0-9]+$/.test(a)?parseInt(a,16):null}
Na(Gl);var Ll={bluetooth:"CONN_DISCO",cellular:"CONN_CELLULAR_UNKNOWN",ethernet:"CONN_WIFI",none:"CONN_NONE",wifi:"CONN_WIFI",wimax:"CONN_CELLULAR_4G",other:"CONN_UNKNOWN",unknown:"CONN_UNKNOWN","slow-2g":"CONN_CELLULAR_2G","2g":"CONN_CELLULAR_2G","3g":"CONN_CELLULAR_3G","4g":"CONN_CELLULAR_4G"},Ml={CONN_DEFAULT:0,CONN_UNKNOWN:1,CONN_NONE:2,CONN_WIFI:3,CONN_CELLULAR_2G:4,CONN_CELLULAR_3G:5,CONN_CELLULAR_4G:6,CONN_CELLULAR_UNKNOWN:7,CONN_DISCO:8,CONN_CELLULAR_5G:9,CONN_WIFI_METERED:10,CONN_CELLULAR_5G_SA:11,
CONN_CELLULAR_5G_NSA:12,CONN_INVALID:31},Nl={EFFECTIVE_CONNECTION_TYPE_UNKNOWN:0,EFFECTIVE_CONNECTION_TYPE_OFFLINE:1,EFFECTIVE_CONNECTION_TYPE_SLOW_2G:2,EFFECTIVE_CONNECTION_TYPE_2G:3,EFFECTIVE_CONNECTION_TYPE_3G:4,EFFECTIVE_CONNECTION_TYPE_4G:5},Ol={"slow-2g":"EFFECTIVE_CONNECTION_TYPE_SLOW_2G","2g":"EFFECTIVE_CONNECTION_TYPE_2G","3g":"EFFECTIVE_CONNECTION_TYPE_3G","4g":"EFFECTIVE_CONNECTION_TYPE_4G"};function Pl(){var a=z.navigator;return a?a.connection:void 0}
function Ql(){var a=Pl();if(a){var b=Ll[a.type||"unknown"]||"CONN_UNKNOWN";a=Ll[a.effectiveType||"unknown"]||"CONN_UNKNOWN";"CONN_CELLULAR_UNKNOWN"===b&&"CONN_UNKNOWN"!==a&&(b=a);if("CONN_UNKNOWN"!==b)return b;if("CONN_UNKNOWN"!==a)return a}}
function Rl(){var a=Pl();if(null!=a&&a.effectiveType)return Ol.hasOwnProperty(a.effectiveType)?Ol[a.effectiveType]:"EFFECTIVE_CONNECTION_TYPE_UNKNOWN"}
;function Sl(){}
function Tl(a,b){return Ul(a,0,b)}
Sl.prototype.R=function(a,b){return Ul(a,1,b)};
function Vl(a,b){Ul(a,2,b)}
function Wl(a){var b=B("yt.scheduler.instance.addImmediateJob");b?b(a):a()}
;function Xl(){Sl.apply(this,arguments)}
u(Xl,Sl);function Yl(){Xl.h||(Xl.h=new Xl);return Xl.h}
function Ul(a,b,c){void 0!==c&&Number.isNaN(Number(c))&&(c=void 0);var d=B("yt.scheduler.instance.addJob");return d?d(a,b,c):void 0===c?(a(),NaN):ml(a,c||0)}
Xl.prototype.ea=function(a){if(void 0===a||!Number.isNaN(Number(a))){var b=B("yt.scheduler.instance.cancelJob");b?b(a):window.clearTimeout(a)}};
Xl.prototype.start=function(){var a=B("yt.scheduler.instance.start");a&&a()};
Xl.prototype.pause=function(){var a=B("yt.scheduler.instance.pause");a&&a()};var bi=Yl();function P(a){var b=Ja.apply(1,arguments);var c=Error.call(this,a);this.message=c.message;"stack"in c&&(this.stack=c.stack);this.args=[].concat(ia(b))}
u(P,Error);function Zl(){try{return $l(),!0}catch(a){return!1}}
function $l(a){if(void 0!==L("DATASYNC_ID"))return L("DATASYNC_ID");throw new P("Datasync ID not set",void 0===a?"unknown":a);}
;var am=Wc||Xc;function bm(a){var b=Sb();return b?0<=b.toLowerCase().indexOf(a):!1}
;function cm(a){var b=new Ii;(b=b.isAvailable()?a?new Oi(b,a):b:null)||(a=new Ji(a||"UserDataSharedStore"),b=a.isAvailable()?a:null);this.h=(a=b)?new Ei(a):null;this.i=document.domain||window.location.hostname}
cm.prototype.set=function(a,b,c,d){c=c||31104E3;this.remove(a);if(this.h)try{this.h.set(a,b,Date.now()+1E3*c);return}catch(f){}var e="";if(d)try{e=escape(Dg(b))}catch(f){return}else e=escape(b);Cl(a,e,c,this.i)};
cm.prototype.get=function(a,b){var c=void 0,d=!this.h;if(!d)try{c=this.h.get(a)}catch(e){d=!0}if(d&&(c=vg.get(""+a,void 0))&&(c=unescape(c),b))try{c=JSON.parse(c)}catch(e){this.remove(a),c=void 0}return c};
cm.prototype.remove=function(a){this.h&&this.h.remove(a);Dl(a,"/",this.i)};var dm=function(){var a;return function(){a||(a=new cm("ytidb"));return a}}();
function em(){var a;return null==(a=dm())?void 0:a.get("LAST_RESULT_ENTRY_KEY",!0)}
;var fm=[],gm,hm=!1;function im(){var a={};for(gm=new jm(void 0===a.handleError?km:a.handleError,void 0===a.logEvent?lm:a.logEvent);0<fm.length;)switch(a=fm.shift(),a.type){case "ERROR":gm.handleError(a.payload);break;case "EVENT":gm.logEvent(a.eventType,a.payload)}}
function mm(a){hm||(gm?gm.handleError(a):(fm.push({type:"ERROR",payload:a}),10<fm.length&&fm.shift()))}
function nm(a,b){hm||(gm?gm.logEvent(a,b):(fm.push({type:"EVENT",eventType:a,payload:b}),10<fm.length&&fm.shift()))}
;function om(a){if(0<=a.indexOf(":"))throw Error("Database name cannot contain ':'");}
function pm(a){return a.substr(0,a.indexOf(":"))||a}
;var qm={},rm=(qm.AUTH_INVALID="No user identifier specified.",qm.EXPLICIT_ABORT="Transaction was explicitly aborted.",qm.IDB_NOT_SUPPORTED="IndexedDB is not supported.",qm.MISSING_INDEX="Index not created.",qm.MISSING_OBJECT_STORES="Object stores not created.",qm.DB_DELETED_BY_MISSING_OBJECT_STORES="Database is deleted because expected object stores were not created.",qm.DB_REOPENED_BY_MISSING_OBJECT_STORES="Database is reopened because expected object stores were not created.",qm.UNKNOWN_ABORT="Transaction was aborted for unknown reasons.",
qm.QUOTA_EXCEEDED="The current transaction exceeded its quota limitations.",qm.QUOTA_MAYBE_EXCEEDED="The current transaction may have failed because of exceeding quota limitations.",qm.EXECUTE_TRANSACTION_ON_CLOSED_DB="Can't start a transaction on a closed database",qm.INCOMPATIBLE_DB_VERSION="The binary is incompatible with the database version",qm),sm={},tm=(sm.AUTH_INVALID="ERROR",sm.EXECUTE_TRANSACTION_ON_CLOSED_DB="WARNING",sm.EXPLICIT_ABORT="IGNORED",sm.IDB_NOT_SUPPORTED="ERROR",sm.MISSING_INDEX=
"WARNING",sm.MISSING_OBJECT_STORES="ERROR",sm.DB_DELETED_BY_MISSING_OBJECT_STORES="WARNING",sm.DB_REOPENED_BY_MISSING_OBJECT_STORES="WARNING",sm.QUOTA_EXCEEDED="WARNING",sm.QUOTA_MAYBE_EXCEEDED="WARNING",sm.UNKNOWN_ABORT="WARNING",sm.INCOMPATIBLE_DB_VERSION="WARNING",sm),um={},vm=(um.AUTH_INVALID=!1,um.EXECUTE_TRANSACTION_ON_CLOSED_DB=!1,um.EXPLICIT_ABORT=!1,um.IDB_NOT_SUPPORTED=!1,um.MISSING_INDEX=!1,um.MISSING_OBJECT_STORES=!1,um.DB_DELETED_BY_MISSING_OBJECT_STORES=!1,um.DB_REOPENED_BY_MISSING_OBJECT_STORES=
!1,um.QUOTA_EXCEEDED=!1,um.QUOTA_MAYBE_EXCEEDED=!0,um.UNKNOWN_ABORT=!0,um.INCOMPATIBLE_DB_VERSION=!1,um);function wm(a,b,c,d,e){b=void 0===b?{}:b;c=void 0===c?rm[a]:c;d=void 0===d?tm[a]:d;e=void 0===e?vm[a]:e;P.call(this,c,Object.assign({},{name:"YtIdbKnownError",isSw:void 0===self.document,isIframe:self!==self.top,type:a},b));this.type=a;this.message=c;this.level=d;this.h=e;Object.setPrototypeOf(this,wm.prototype)}
u(wm,P);function xm(a,b){wm.call(this,"MISSING_OBJECT_STORES",{expectedObjectStores:b,foundObjectStores:a},rm.MISSING_OBJECT_STORES);Object.setPrototypeOf(this,xm.prototype)}
u(xm,wm);function ym(a,b){var c=Error.call(this);this.message=c.message;"stack"in c&&(this.stack=c.stack);this.index=a;this.objectStore=b;Object.setPrototypeOf(this,ym.prototype)}
u(ym,Error);var zm=["The database connection is closing","Can't start a transaction on a closed database","A mutation operation was attempted on a database that did not allow mutations"];
function Am(a,b,c,d){b=pm(b);var e=a instanceof Error?a:Error("Unexpected error: "+a);if(e instanceof wm)return e;a={objectStoreNames:c,dbName:b,dbVersion:d};if("QuotaExceededError"===e.name)return new wm("QUOTA_EXCEEDED",a);if(Yc&&"UnknownError"===e.name)return new wm("QUOTA_MAYBE_EXCEEDED",a);if(e instanceof ym)return new wm("MISSING_INDEX",Object.assign({},a,{objectStore:e.objectStore,index:e.index}));if("InvalidStateError"===e.name&&zm.some(function(f){return e.message.includes(f)}))return new wm("EXECUTE_TRANSACTION_ON_CLOSED_DB",
a);
if("AbortError"===e.name)return new wm("UNKNOWN_ABORT",a,e.message);e.args=[Object.assign({},a,{name:"IdbError",jc:e.name})];e.level="WARNING";return e}
function Bm(a,b,c){var d=em();return new wm("IDB_NOT_SUPPORTED",{context:{caller:a,publicName:b,version:c,hasSucceededOnce:null==d?void 0:d.hasSucceededOnce}})}
;function Cm(a){if(!a)throw Error();throw a;}
function Dm(a){return a}
function Em(a){this.h=a}
function Fm(a){function b(e){if("PENDING"===d.state.status){d.state={status:"REJECTED",reason:e};e=p(d.i);for(var f=e.next();!f.done;f=e.next())f=f.value,f()}}
function c(e){if("PENDING"===d.state.status){d.state={status:"FULFILLED",value:e};e=p(d.h);for(var f=e.next();!f.done;f=e.next())f=f.value,f()}}
var d=this;this.state={status:"PENDING"};this.h=[];this.i=[];a=a.h;try{a(c,b)}catch(e){b(e)}}
Fm.all=function(a){return new Fm(new Em(function(b,c){var d=[],e=a.length;0===e&&b(d);for(var f={za:0};f.za<a.length;f={za:f.za},++f.za)Fm.resolve(a[f.za]).then(function(g){return function(h){d[g.za]=h;e--;0===e&&b(d)}}(f)).catch(function(g){c(g)})}))};
Fm.resolve=function(a){return new Fm(new Em(function(b,c){a instanceof Fm?a.then(b,c):b(a)}))};
Fm.reject=function(a){return new Fm(new Em(function(b,c){c(a)}))};
Fm.prototype.then=function(a,b){var c=this,d=null!=a?a:Dm,e=null!=b?b:Cm;return new Fm(new Em(function(f,g){"PENDING"===c.state.status?(c.h.push(function(){Gm(c,c,d,f,g)}),c.i.push(function(){Hm(c,c,e,f,g)})):"FULFILLED"===c.state.status?Gm(c,c,d,f,g):"REJECTED"===c.state.status&&Hm(c,c,e,f,g)}))};
Fm.prototype.catch=function(a){return this.then(void 0,a)};
function Gm(a,b,c,d,e){try{if("FULFILLED"!==a.state.status)throw Error("calling handleResolve before the promise is fulfilled.");var f=c(a.state.value);f instanceof Fm?Im(a,b,f,d,e):d(f)}catch(g){e(g)}}
function Hm(a,b,c,d,e){try{if("REJECTED"!==a.state.status)throw Error("calling handleReject before the promise is rejected.");var f=c(a.state.reason);f instanceof Fm?Im(a,b,f,d,e):d(f)}catch(g){e(g)}}
function Im(a,b,c,d,e){b===c?e(new TypeError("Circular promise chain detected.")):c.then(function(f){f instanceof Fm?Im(a,b,f,d,e):d(f)},function(f){e(f)})}
;function Jm(a,b,c){function d(){c(a.error);f()}
function e(){b(a.result);f()}
function f(){try{a.removeEventListener("success",e),a.removeEventListener("error",d)}catch(g){}}
a.addEventListener("success",e);a.addEventListener("error",d)}
function Km(a){return new Promise(function(b,c){Jm(a,b,c)})}
function Lm(a){return new Fm(new Em(function(b,c){Jm(a,b,c)}))}
;function Mm(a,b){return new Fm(new Em(function(c,d){function e(){var f=a?b(a):null;f?f.then(function(g){a=g;e()},d):c()}
e()}))}
;var Nm=window,R=Nm.ytcsi&&Nm.ytcsi.now?Nm.ytcsi.now:Nm.performance&&Nm.performance.timing&&Nm.performance.now&&Nm.performance.timing.navigationStart?function(){return Nm.performance.timing.navigationStart+Nm.performance.now()}:function(){return(new Date).getTime()};function Om(a,b){this.h=a;this.options=b;this.transactionCount=0;this.j=Math.round(R());this.i=!1}
l=Om.prototype;l.add=function(a,b,c){return Pm(this,[a],{mode:"readwrite",T:!0},function(d){return d.objectStore(a).add(b,c)})};
l.clear=function(a){return Pm(this,[a],{mode:"readwrite",T:!0},function(b){return b.objectStore(a).clear()})};
l.close=function(){this.h.close();var a;(null==(a=this.options)?0:a.closed)&&this.options.closed()};
l.count=function(a,b){return Pm(this,[a],{mode:"readonly",T:!0},function(c){return c.objectStore(a).count(b)})};
function Qm(a,b,c){a=a.h.createObjectStore(b,c);return new Rm(a)}
l.delete=function(a,b){return Pm(this,[a],{mode:"readwrite",T:!0},function(c){return c.objectStore(a).delete(b)})};
l.get=function(a,b){return Pm(this,[a],{mode:"readonly",T:!0},function(c){return c.objectStore(a).get(b)})};
function Sm(a,b,c){return Pm(a,[b],{mode:"readwrite",T:!0},function(d){d=d.objectStore(b);return Lm(d.h.put(c,void 0))})}
l.objectStoreNames=function(){return Array.from(this.h.objectStoreNames)};
function Pm(a,b,c,d){var e,f,g,h,k,m,q,r,w,t,y,E;return x(function(F){switch(F.h){case 1:var O={mode:"readonly",T:!1,tag:"IDB_TRANSACTION_TAG_UNKNOWN"};"string"===typeof c?O.mode=c:Object.assign(O,c);e=O;a.transactionCount++;f=e.T?3:1;g=0;case 2:if(h){F.u(3);break}g++;k=Math.round(R());ya(F,4);m=a.h.transaction(b,e.mode);O=new Tm(m);O=Um(O,d);return v(F,O,6);case 6:return q=F.i,r=Math.round(R()),Vm(a,k,r,g,void 0,b.join(),e),F.return(q);case 4:w=Aa(F);t=Math.round(R());y=Am(w,a.h.name,b.join(),a.h.version);
if((E=y instanceof wm&&!y.h)||g>=f)Vm(a,k,t,g,y,b.join(),e),h=y;F.u(2);break;case 3:return F.return(Promise.reject(h))}})}
function Vm(a,b,c,d,e,f,g){b=c-b;e?(e instanceof wm&&("QUOTA_EXCEEDED"===e.type||"QUOTA_MAYBE_EXCEEDED"===e.type)&&nm("QUOTA_EXCEEDED",{dbName:pm(a.h.name),objectStoreNames:f,transactionCount:a.transactionCount,transactionMode:g.mode}),e instanceof wm&&"UNKNOWN_ABORT"===e.type&&(c-=a.j,0>c&&c>=Math.pow(2,31)&&(c=0),nm("TRANSACTION_UNEXPECTEDLY_ABORTED",{objectStoreNames:f,transactionDuration:b,transactionCount:a.transactionCount,dbDuration:c}),a.i=!0),Wm(a,!1,d,f,b,g.tag),mm(e)):Wm(a,!0,d,f,b,g.tag)}
function Wm(a,b,c,d,e,f){nm("TRANSACTION_ENDED",{objectStoreNames:d,connectionHasUnknownAbortedTransaction:a.i,duration:e,isSuccessful:b,tryCount:c,tag:void 0===f?"IDB_TRANSACTION_TAG_UNKNOWN":f})}
l.getName=function(){return this.h.name};
function Rm(a){this.h=a}
l=Rm.prototype;l.add=function(a,b){return Lm(this.h.add(a,b))};
l.autoIncrement=function(){return this.h.autoIncrement};
l.clear=function(){return Lm(this.h.clear()).then(function(){})};
function Xm(a,b,c){a.h.createIndex(b,c,{unique:!1})}
l.count=function(a){return Lm(this.h.count(a))};
function Ym(a,b){return Zm(a,{query:b},function(c){return c.delete().then(function(){return c.continue()})}).then(function(){})}
l.delete=function(a){return a instanceof IDBKeyRange?Ym(this,a):Lm(this.h.delete(a))};
l.get=function(a){return Lm(this.h.get(a))};
l.index=function(a){try{return new $m(this.h.index(a))}catch(b){if(b instanceof Error&&"NotFoundError"===b.name)throw new ym(a,this.h.name);throw b;}};
l.getName=function(){return this.h.name};
l.keyPath=function(){return this.h.keyPath};
function Zm(a,b,c){a=a.h.openCursor(b.query,b.direction);return an(a).then(function(d){return Mm(d,c)})}
function Tm(a){var b=this;this.h=a;this.j=new Map;this.i=!1;this.done=new Promise(function(c,d){b.h.addEventListener("complete",function(){c()});
b.h.addEventListener("error",function(e){e.currentTarget===e.target&&d(b.h.error)});
b.h.addEventListener("abort",function(){var e=b.h.error;if(e)d(e);else if(!b.i){e=wm;for(var f=b.h.objectStoreNames,g=[],h=0;h<f.length;h++){var k=f.item(h);if(null===k)throw Error("Invariant: item in DOMStringList is null");g.push(k)}e=new e("UNKNOWN_ABORT",{objectStoreNames:g.join(),dbName:b.h.db.name,mode:b.h.mode});d(e)}})})}
function Um(a,b){var c=new Promise(function(d,e){try{b(a).then(function(f){d(f)}).catch(e)}catch(f){e(f),a.abort()}});
return Promise.all([c,a.done]).then(function(d){return p(d).next().value})}
Tm.prototype.abort=function(){this.h.abort();this.i=!0;throw new wm("EXPLICIT_ABORT");};
Tm.prototype.objectStore=function(a){a=this.h.objectStore(a);var b=this.j.get(a);b||(b=new Rm(a),this.j.set(a,b));return b};
function $m(a){this.h=a}
l=$m.prototype;l.count=function(a){return Lm(this.h.count(a))};
l.delete=function(a){return bn(this,{query:a},function(b){return b.delete().then(function(){return b.continue()})})};
l.get=function(a){return Lm(this.h.get(a))};
l.getKey=function(a){return Lm(this.h.getKey(a))};
l.keyPath=function(){return this.h.keyPath};
l.unique=function(){return this.h.unique};
function bn(a,b,c){a=a.h.openCursor(void 0===b.query?null:b.query,void 0===b.direction?"next":b.direction);return an(a).then(function(d){return Mm(d,c)})}
function cn(a,b){this.request=a;this.cursor=b}
function an(a){return Lm(a).then(function(b){return b?new cn(a,b):null})}
l=cn.prototype;l.advance=function(a){this.cursor.advance(a);return an(this.request)};
l.continue=function(a){this.cursor.continue(a);return an(this.request)};
l.delete=function(){return Lm(this.cursor.delete()).then(function(){})};
l.getKey=function(){return this.cursor.key};
l.getValue=function(){return this.cursor.value};
l.update=function(a){return Lm(this.cursor.update(a))};function dn(a,b,c){return new Promise(function(d,e){function f(){w||(w=new Om(g.result,{closed:r}));return w}
var g=void 0!==b?self.indexedDB.open(a,b):self.indexedDB.open(a);var h=c.Hc,k=c.blocking,m=c.xd,q=c.upgrade,r=c.closed,w;g.addEventListener("upgradeneeded",function(t){try{if(null===t.newVersion)throw Error("Invariant: newVersion on IDbVersionChangeEvent is null");if(null===g.transaction)throw Error("Invariant: transaction on IDbOpenDbRequest is null");t.dataLoss&&"none"!==t.dataLoss&&nm("IDB_DATA_CORRUPTED",{reason:t.dataLossMessage||"unknown reason",dbName:pm(a)});var y=f(),E=new Tm(g.transaction);
q&&q(y,function(F){return t.oldVersion<F&&t.newVersion>=F},E);
E.done.catch(function(F){e(F)})}catch(F){e(F)}});
g.addEventListener("success",function(){var t=g.result;k&&t.addEventListener("versionchange",function(){k(f())});
t.addEventListener("close",function(){nm("IDB_UNEXPECTEDLY_CLOSED",{dbName:pm(a),dbVersion:t.version});m&&m()});
d(f())});
g.addEventListener("error",function(){e(g.error)});
h&&g.addEventListener("blocked",function(){h()})})}
function en(a,b,c){c=void 0===c?{}:c;return dn(a,b,c)}
function fn(a,b){b=void 0===b?{}:b;var c,d,e,f;return x(function(g){if(1==g.h)return ya(g,2),c=self.indexedDB.deleteDatabase(a),d=b,(e=d.Hc)&&c.addEventListener("blocked",function(){e()}),v(g,Km(c),4);
if(2!=g.h)return za(g,0);f=Aa(g);throw Am(f,a,"",-1);})}
;function gn(a){return new Promise(function(b){Vl(function(){b()},a)})}
function hn(a,b){this.name=a;this.options=b;this.l=!0;this.o=this.m=0;this.i=500}
hn.prototype.j=function(a,b,c){c=void 0===c?{}:c;return en(a,b,c)};
hn.prototype.delete=function(a){a=void 0===a?{}:a;return fn(this.name,a)};
function jn(a,b){return new wm("INCOMPATIBLE_DB_VERSION",{dbName:a.name,oldVersion:a.options.version,newVersion:b})}
function kn(a,b){if(!b)throw Bm("openWithToken",pm(a.name));return ln(a)}
function ln(a){function b(){var f,g,h,k,m,q,r,w,t,y;return x(function(E){switch(E.h){case 1:return g=null!=(f=Error().stack)?f:"",ya(E,2),v(E,a.j(a.name,a.options.version,d),4);case 4:h=E.i;for(var F=a.options,O=[],N=p(Object.keys(F.Da)),Q=N.next();!Q.done;Q=N.next()){Q=Q.value;var da=F.Da[Q],U=void 0===da.hd?Number.MAX_VALUE:da.hd;!(h.h.version>=da.Ia)||h.h.version>=U||h.h.objectStoreNames.contains(Q)||O.push(Q)}k=O;if(0===k.length){E.u(5);break}m=Object.keys(a.options.Da);q=h.objectStoreNames();
if(a.o<Nk("ytidb_reopen_db_retries",0))return a.o++,h.close(),mm(new wm("DB_REOPENED_BY_MISSING_OBJECT_STORES",{dbName:a.name,expectedObjectStores:m,foundObjectStores:q})),E.return(b());if(!(a.m<Nk("ytidb_remake_db_retries",1))){E.u(6);break}a.m++;if(!M("ytidb_remake_db_enable_backoff_delay")){E.u(7);break}return v(E,gn(a.i),8);case 8:a.i*=2;case 7:return v(E,a.delete(),9);case 9:return mm(new wm("DB_DELETED_BY_MISSING_OBJECT_STORES",{dbName:a.name,expectedObjectStores:m,foundObjectStores:q})),E.return(b());
case 6:throw new xm(q,m);case 5:return E.return(h);case 2:r=Aa(E);if(r instanceof DOMException?"VersionError"!==r.name:"DOMError"in self&&r instanceof DOMError?"VersionError"!==r.name:!(r instanceof Object&&"message"in r)||"An attempt was made to open a database using a lower version than the existing version."!==r.message){E.u(10);break}return v(E,a.j(a.name,void 0,Object.assign({},d,{upgrade:void 0})),11);case 11:w=E.i;t=w.h.version;if(void 0!==a.options.version&&t>a.options.version+1)throw w.close(),
a.l=!1,jn(a,t);return E.return(w);case 10:throw c(),r instanceof Error&&!M("ytidb_async_stack_killswitch")&&(r.stack=r.stack+"\n"+g.substring(g.indexOf("\n")+1)),Am(r,a.name,"",null!=(y=a.options.version)?y:-1);}})}
function c(){a.h===e&&(a.h=void 0)}
if(!a.l)throw jn(a);if(a.h)return a.h;var d={blocking:function(f){f.close()},
closed:c,xd:c,upgrade:a.options.upgrade};var e=b();a.h=e;return a.h}
;var mn=new hn("YtIdbMeta",{Da:{databases:{Ia:1}},upgrade:function(a,b){b(1)&&Qm(a,"databases",{keyPath:"actualName"})}});
function nn(a,b){var c;return x(function(d){if(1==d.h)return v(d,kn(mn,b),2);c=d.i;return d.return(Pm(c,["databases"],{T:!0,mode:"readwrite"},function(e){var f=e.objectStore("databases");return f.get(a.actualName).then(function(g){if(g?a.actualName!==g.actualName||a.publicName!==g.publicName||a.userIdentifier!==g.userIdentifier:1)return Lm(f.h.put(a,void 0)).then(function(){})})}))})}
function on(a,b){var c;return x(function(d){if(1==d.h)return a?v(d,kn(mn,b),2):d.return();c=d.i;return d.return(c.delete("databases",a))})}
function pn(a,b){var c,d;return x(function(e){return 1==e.h?(c=[],v(e,kn(mn,b),2)):3!=e.h?(d=e.i,v(e,Pm(d,["databases"],{T:!0,mode:"readonly"},function(f){c.length=0;return Zm(f.objectStore("databases"),{},function(g){a(g.getValue())&&c.push(g.getValue());return g.continue()})}),3)):e.return(c)})}
function qn(a){return pn(function(b){return"LogsDatabaseV2"===b.publicName&&void 0!==b.userIdentifier},a)}
function rn(a,b,c){return pn(function(d){return c?void 0!==d.userIdentifier&&!a.includes(d.userIdentifier)&&c.includes(d.publicName):void 0!==d.userIdentifier&&!a.includes(d.userIdentifier)},b)}
function sn(a){var b,c;return x(function(d){if(1==d.h)return b=$l("YtIdbMeta hasAnyMeta other"),v(d,pn(function(e){return void 0!==e.userIdentifier&&e.userIdentifier!==b},a),2);
c=d.i;return d.return(0<c.length)})}
;var tn,un=new function(){}(new function(){});
function vn(){var a,b,c,d;return x(function(e){switch(e.h){case 1:a=em();if(null==(b=a)?0:b.hasSucceededOnce)return e.return(!0);var f;if(f=am)f=/WebKit\/([0-9]+)/.exec(Sb()),f=!!(f&&600<=parseInt(f[1],10));f&&(f=/WebKit\/([0-9]+)/.exec(Sb()),f=!(f&&602<=parseInt(f[1],10)));if(f||Jc)return e.return(!1);try{if(c=self,!(c.indexedDB&&c.IDBIndex&&c.IDBKeyRange&&c.IDBObjectStore))return e.return(!1)}catch(g){return e.return(!1)}if(!("IDBTransaction"in self&&"objectStoreNames"in IDBTransaction.prototype))return e.return(!1);
ya(e,2);d={actualName:"yt-idb-test-do-not-use",publicName:"yt-idb-test-do-not-use",userIdentifier:void 0};return v(e,nn(d,un),4);case 4:return v(e,on("yt-idb-test-do-not-use",un),5);case 5:return e.return(!0);case 2:return Aa(e),e.return(!1)}})}
function wn(){if(void 0!==tn)return tn;hm=!0;return tn=vn().then(function(a){hm=!1;var b;if(null!=(b=dm())&&b.h){var c;b={hasSucceededOnce:(null==(c=em())?void 0:c.hasSucceededOnce)||a};var d;null==(d=dm())||d.set("LAST_RESULT_ENTRY_KEY",b,2592E3,!0)}return a})}
function xn(){return B("ytglobal.idbToken_")||void 0}
function yn(){var a=xn();return a?Promise.resolve(a):wn().then(function(b){(b=b?un:void 0)&&A("ytglobal.idbToken_",b);return b})}
;var zn=0;function An(a,b){zn||(zn=bi.R(function(){var c,d,e,f,g;return x(function(h){switch(h.h){case 1:return v(h,yn(),2);case 2:c=h.i;if(!c)return h.return();d=!0;ya(h,3);return v(h,rn(a,c,b),5);case 5:e=h.i;if(!e.length){d=!1;h.u(6);break}f=e[0];return v(h,fn(f.actualName),7);case 7:return v(h,on(f.actualName,c),6);case 6:za(h,4);break;case 3:g=Aa(h),mm(g),d=!1;case 4:bi.ea(zn),zn=0,d&&An(a,b),h.h=0}})}))}
function Bn(){var a;return x(function(b){return 1==b.h?v(b,yn(),2):(a=b.i)?b.return(sn(a)):b.return(!1)})}
new Gh;function Cn(a){if(!Zl())throw a=new wm("AUTH_INVALID",{dbName:a}),mm(a),a;var b=$l();return{actualName:a+":"+b,publicName:a,userIdentifier:b}}
function Dn(a,b,c,d){var e,f,g,h,k,m;return x(function(q){switch(q.h){case 1:return f=null!=(e=Error().stack)?e:"",v(q,yn(),2);case 2:g=q.i;if(!g)throw h=Bm("openDbImpl",a,b),M("ytidb_async_stack_killswitch")||(h.stack=h.stack+"\n"+f.substring(f.indexOf("\n")+1)),mm(h),h;om(a);k=c?{actualName:a,publicName:a,userIdentifier:void 0}:Cn(a);ya(q,3);return v(q,nn(k,g),5);case 5:return v(q,en(k.actualName,b,d),6);case 6:return q.return(q.i);case 3:return m=Aa(q),ya(q,7),v(q,on(k.actualName,g),9);case 9:za(q,
8);break;case 7:Aa(q);case 8:throw m;}})}
function En(a,b,c){c=void 0===c?{}:c;return Dn(a,b,!1,c)}
function Fn(a,b,c){c=void 0===c?{}:c;return Dn(a,b,!0,c)}
function Gn(a,b){b=void 0===b?{}:b;var c,d;return x(function(e){if(1==e.h)return v(e,yn(),2);if(3!=e.h){c=e.i;if(!c)return e.return();om(a);d=Cn(a);return v(e,fn(d.actualName,b),3)}return v(e,on(d.actualName,c),0)})}
function Hn(a,b,c){a=a.map(function(d){return x(function(e){return 1==e.h?v(e,fn(d.actualName,b),2):v(e,on(d.actualName,c),0)})});
return Promise.all(a).then(function(){})}
function In(){var a=void 0===a?{}:a;var b,c;return x(function(d){if(1==d.h)return v(d,yn(),2);if(3!=d.h){b=d.i;if(!b)return d.return();om("LogsDatabaseV2");return v(d,qn(b),3)}c=d.i;return v(d,Hn(c,a,b),0)})}
function Jn(a,b){b=void 0===b?{}:b;var c;return x(function(d){if(1==d.h)return v(d,yn(),2);if(3!=d.h){c=d.i;if(!c)return d.return();om(a);return v(d,fn(a,b),3)}return v(d,on(a,c),0)})}
;function Kn(a,b){hn.call(this,a,b);this.options=b;om(a)}
u(Kn,hn);function Ln(a,b){var c;return function(){c||(c=new Kn(a,b));return c}}
Kn.prototype.j=function(a,b,c){c=void 0===c?{}:c;return(this.options.pb?Fn:En)(a,b,Object.assign({},c))};
Kn.prototype.delete=function(a){a=void 0===a?{}:a;return(this.options.pb?Jn:Gn)(this.name,a)};
function Mn(a,b){return Ln(a,b)}
;var Nn={},On=Mn("ytGcfConfig",{Da:(Nn.coldConfigStore={Ia:1},Nn.hotConfigStore={Ia:1},Nn),pb:!1,upgrade:function(a,b){b(1)&&(Xm(Qm(a,"hotConfigStore",{keyPath:"key",autoIncrement:!0}),"hotTimestampIndex","timestamp"),Xm(Qm(a,"coldConfigStore",{keyPath:"key",autoIncrement:!0}),"coldTimestampIndex","timestamp"))},
version:1});function Pn(a){return kn(On(),a)}
function Qn(a,b,c){var d,e,f;return x(function(g){switch(g.h){case 1:return d={config:a,hashData:b,timestamp:R()},v(g,Pn(c),2);case 2:return e=g.i,v(g,e.clear("hotConfigStore"),3);case 3:return v(g,Sm(e,"hotConfigStore",d),4);case 4:return f=g.i,g.return(f)}})}
function Rn(a,b,c,d){var e,f,g;return x(function(h){switch(h.h){case 1:return e={config:a,hashData:b,configData:c,timestamp:R()},v(h,Pn(d),2);case 2:return f=h.i,v(h,f.clear("coldConfigStore"),3);case 3:return v(h,Sm(f,"coldConfigStore",e),4);case 4:return g=h.i,h.return(g)}})}
;function Sn(){}
function Tn(a,b,c){var d,e;return x(function(f){if(!M("gcf_config_store_update_enabled"))return f.u(0);a.h=b;A("yt.gcf.config.hotConfigGroup",a.h);a.hotHashData=c;A("yt.gcf.config.hotHashData",a.hotHashData);d=xn();return d?v(f,Qn(b,c,d),0):(e=Bm("updateHotConfig token error"),Uk(e),f.u(0))})}
function Un(a,b,c){var d,e;return x(function(f){if(!M("gcf_config_store_update_enabled"))return f.u(0);a.coldHashData=c;A("yt.gcf.config.coldHashData",a.coldHashData);d=xn();return d?b.configData?v(f,Rn(b,c,b.configData,d),0):f.u(0):(e=Bm("updateColdConfig token error"),Uk(e),f.u(0))})}
;function Vn(a){a:{var b=a.raw_embedded_player_response;if(!b&&(a=a.embedded_player_response))try{b=JSON.parse(a)}catch(d){b="EMBEDDED_PLAYER_MODE_UNKNOWN";break a}if(b)b:{for(var c in mj)if(mj[c]==b.embeddedPlayerMode){b=mj[c];break b}b="EMBEDDED_PLAYER_MODE_UNKNOWN"}else b="EMBEDDED_PLAYER_MODE_UNKNOWN"}return"EMBEDDED_PLAYER_MODE_PFL"===b}
;function Wn(){return"INNERTUBE_API_KEY"in Jk&&"INNERTUBE_API_VERSION"in Jk}
function Xn(){return{innertubeApiKey:L("INNERTUBE_API_KEY"),innertubeApiVersion:L("INNERTUBE_API_VERSION"),yb:L("INNERTUBE_CONTEXT_CLIENT_CONFIG_INFO"),cc:L("INNERTUBE_CONTEXT_CLIENT_NAME","WEB"),Uc:L("INNERTUBE_CONTEXT_CLIENT_NAME",1),innertubeContextClientVersion:L("INNERTUBE_CONTEXT_CLIENT_VERSION"),ec:L("INNERTUBE_CONTEXT_HL"),dc:L("INNERTUBE_CONTEXT_GL"),Vc:L("INNERTUBE_HOST_OVERRIDE")||"",Xc:!!L("INNERTUBE_USE_THIRD_PARTY_AUTH",!1),Wc:!!L("INNERTUBE_OMIT_API_KEY_WHEN_AUTH_HEADER_IS_PRESENT",
!1),appInstallData:L("SERIALIZED_CLIENT_CONFIG_DATA")}}
function Yn(a){var b={client:{hl:a.ec,gl:a.dc,clientName:a.cc,clientVersion:a.innertubeContextClientVersion,configInfo:a.yb}};navigator.userAgent&&(b.client.userAgent=String(navigator.userAgent));var c=z.devicePixelRatio;c&&1!=c&&(b.client.screenDensityFloat=String(c));c=Ok();""!==c&&(b.client.experimentsToken=c);c=Pk();0<c.length&&(b.request={internalExperimentFlags:c});Zn(a,void 0,b);M("enable_third_party_info")&&$n(void 0,b);ao(void 0,b);bo(a,void 0,b);co(void 0,b);M("start_sending_config_hash")&&
eo(void 0,b);L("DELEGATED_SESSION_ID")&&!M("pageid_as_header_web")&&(b.user={onBehalfOfUser:L("DELEGATED_SESSION_ID")});a=Object;c=a.assign;for(var d=b.client,e={},f=p(Object.entries(dl(L("DEVICE","")))),g=f.next();!g.done;g=f.next()){var h=p(g.value);g=h.next().value;h=h.next().value;"cbrand"===g?e.deviceMake=h:"cmodel"===g?e.deviceModel=h:"cbr"===g?e.browserName=h:"cbrver"===g?e.browserVersion=h:"cos"===g?e.osName=h:"cosver"===g?e.osVersion=h:"cplatform"===g&&(e.platform=h)}b.client=c.call(a,d,
e);return b}
function fo(a){var b=new hj,c=new Zi;D(c,1,a.ec);D(c,2,a.dc);D(c,16,a.Uc);D(c,17,a.innertubeContextClientVersion);if(a.yb){var d=a.yb,e=new Vi;d.coldConfigData&&D(e,1,d.coldConfigData);d.appInstallData&&D(e,6,d.appInstallData);d.coldHashData&&D(e,3,d.coldHashData);d.hotHashData&&D(e,5,d.hotHashData);G(c,Vi,62,e)}(d=z.devicePixelRatio)&&1!=d&&D(c,65,d);d=Ok();""!==d&&D(c,54,d);d=Pk();if(0<d.length){e=new aj;for(var f=0;f<d.length;f++){var g=new Ti;D(g,1,d[f].key);Xd(g,2,Ui,d[f].value);ee(e,15,Ti,g)}G(b,
aj,5,e)}Zn(a,c);M("enable_third_party_info")&&$n(b);ao(c);bo(a,c);co(c);M("start_sending_config_hash")&&eo(c);L("DELEGATED_SESSION_ID")&&!M("pageid_as_header_web")&&(a=new fj,D(a,3,L("DELEGATED_SESSION_ID")));a=p(Object.entries(dl(L("DEVICE",""))));for(d=a.next();!d.done;d=a.next())e=p(d.value),d=e.next().value,e=e.next().value,"cbrand"===d?D(c,12,e):"cmodel"===d?D(c,13,e):"cbr"===d?D(c,87,e):"cbrver"===d?D(c,88,e):"cos"===d?D(c,18,e):"cosver"===d?D(c,19,e):"cplatform"===d&&D(c,42,e);b.m(c);return b}
function Zn(a,b,c){a=a.cc;if("WEB"===a||"MWEB"===a||1===a||2===a)if(b){c=Zd(b,Wi,96)||new Wi;var d=Bl();d=Object.keys(oj).indexOf(d);d=-1===d?null:d;null!==d&&D(c,3,d);G(b,Wi,96,c)}else c&&(c.client.mainAppWebInfo=null!=(d=c.client.mainAppWebInfo)?d:{},c.client.mainAppWebInfo.webDisplayMode=Bl())}
function $n(a,b){var c=B("yt.embedded_player.embed_url");c&&(a?(b=Zd(a,cj,7)||new cj,D(b,4,c),G(a,cj,7,b)):b&&(b.thirdParty={embedUrl:c}))}
function ao(a,b){var c;if(M("web_log_memory_total_kbytes")&&(null==(c=z.navigator)?0:c.deviceMemory)){var d;c=null==(d=z.navigator)?void 0:d.deviceMemory;a?D(a,95,1E6*c):b&&(b.client.memoryTotalKbytes=""+1E6*c)}}
function bo(a,b,c){if(a.appInstallData)if(b){var d;c=null!=(d=Zd(b,Vi,62))?d:new Vi;D(c,6,a.appInstallData);G(b,Vi,62,c)}else c&&(c.client.configInfo=c.client.configInfo||{},c.client.configInfo.appInstallData=a.appInstallData)}
function co(a,b){var c=Ql();c&&(a?D(a,61,Ml[c]):b&&(b.client.connectionType=c));M("web_log_effective_connection_type")&&(c=Rl())&&(a?D(a,94,Nl[c]):b&&(b.client.effectiveConnectionType=c))}
function go(a,b,c){c=void 0===c?{}:c;var d={};L("EOM_VISITOR_DATA")?d={"X-Goog-EOM-Visitor-Id":L("EOM_VISITOR_DATA")}:d={"X-Goog-Visitor-Id":c.visitorData||L("VISITOR_DATA","")};if(b&&b.includes("www.youtube-nocookie.com"))return d;(b=c.ir||L("AUTHORIZATION"))||(a?b="Bearer "+B("gapi.auth.getToken")().hr:b=zg([]));b&&(d.Authorization=b,d["X-Goog-AuthUser"]=L("SESSION_INDEX",0),M("pageid_as_header_web")&&(d["X-Goog-PageId"]=L("DELEGATED_SESSION_ID")));return d}
function eo(a,b){Sn.h||(Sn.h=new Sn);var c=B("yt.gcf.config.coldConfigData");var d=B("yt.gcf.config.hotHashData");var e=B("yt.gcf.config.coldHashData");if(c&&e&&d)if(a){var f;b=null!=(f=Zd(a,Vi,62))?f:new Vi;D(b,1,c);D(b,3,e);D(b,5,d);G(a,Vi,62,b)}else b&&(b.client.configInfo=b.client.configInfo||{},b.client.configInfo.coldConfigData=c,b.client.configInfo.coldHashData=e,b.client.configInfo.hotHashData=d)}
;function ho(a){a=Object.assign({},a);delete a.Authorization;var b=zg();if(b){var c=new hi;c.update(L("INNERTUBE_API_KEY"));c.update(b);a.hash=ad(c.digest(),3)}return a}
;var io;function jo(){io||(io=new cm("yt.innertube"));return io}
function ko(a,b,c,d){if(d)return null;d=jo().get("nextId",!0)||1;var e=jo().get("requests",!0)||{};e[d]={method:a,request:b,authState:ho(c),requestTime:Math.round(R())};jo().set("nextId",d+1,86400,!0);jo().set("requests",e,86400,!0);return d}
function lo(a){var b=jo().get("requests",!0)||{};delete b[a];jo().set("requests",b,86400,!0)}
function mo(a){var b=jo().get("requests",!0);if(b){for(var c in b){var d=b[c];if(!(6E4>Math.round(R())-d.requestTime)){var e=d.authState,f=ho(go(!1));tb(e,f)&&(e=d.request,"requestTimeMs"in e&&(e.requestTimeMs=Math.round(R())),no(a,d.method,e,{}));delete b[c]}}jo().set("requests",b,86400,!0)}}
;function oo(a){this.bb=this.h=!1;this.potentialEsfErrorCounter=this.i=0;this.handleError=function(){};
this.Ca=function(){};
this.now=Date.now;this.Ka=!1;var b;this.uc=null!=(b=a.uc)?b:100;var c;this.oc=null!=(c=a.oc)?c:1;var d;this.mc=null!=(d=a.mc)?d:2592E6;var e;this.kc=null!=(e=a.kc)?e:12E4;var f;this.nc=null!=(f=a.nc)?f:5E3;var g;this.H=null!=(g=a.H)?g:void 0;this.gb=!!a.gb;var h;this.fb=null!=(h=a.fb)?h:.1;var k;this.lb=null!=(k=a.lb)?k:10;a.handleError&&(this.handleError=a.handleError);a.Ca&&(this.Ca=a.Ca);a.Ka&&(this.Ka=a.Ka);a.bb&&(this.bb=a.bb);this.J=a.J;this.Y=a.Y;this.N=a.N;this.L=a.L;this.ia=a.ia;this.Db=
a.Db;this.Cb=a.Cb;po(this)&&(!this.J||this.J("networkless_logging"))&&qo(this)}
function qo(a){po(a)&&!a.Ka&&(a.h=!0,a.gb&&Math.random()<=a.fb&&a.N.Jc(a.H),ro(a),a.L.U()&&a.Ra(),a.L.ha(a.Db,a.Ra.bind(a)),a.L.ha(a.Cb,a.Qb.bind(a)))}
l=oo.prototype;l.writeThenSend=function(a,b){var c=this;b=void 0===b?{}:b;if(po(this)&&this.h){var d={url:a,options:b,timestamp:this.now(),status:"NEW",sendCount:0};this.N.set(d,this.H).then(function(e){d.id=e;c.L.U()&&so(c,d)}).catch(function(e){so(c,d);
to(c,e)})}else this.ia(a,b)};
l.sendThenWrite=function(a,b,c){var d=this;b=void 0===b?{}:b;if(po(this)&&this.h){var e={url:a,options:b,timestamp:this.now(),status:"NEW",sendCount:0};this.J&&this.J("nwl_skip_retry")&&(e.skipRetry=c);if(this.L.U()||this.J&&this.J("nwl_aggressive_send_then_write")&&!e.skipRetry){if(!e.skipRetry){var f=b.onError?b.onError:function(){};
b.onError=function(g,h){return x(function(k){if(1==k.h)return v(k,d.N.set(e,d.H).catch(function(m){to(d,m)}),2);
f(g,h);k.h=0})}}this.ia(a,b,e.skipRetry)}else this.N.set(e,this.H).catch(function(g){d.ia(a,b,e.skipRetry);
to(d,g)})}else this.ia(a,b,this.J&&this.J("nwl_skip_retry")&&c)};
l.sendAndWrite=function(a,b){var c=this;b=void 0===b?{}:b;if(po(this)&&this.h){var d={url:a,options:b,timestamp:this.now(),status:"NEW",sendCount:0},e=!1,f=b.onSuccess?b.onSuccess:function(){};
d.options.onSuccess=function(g,h){void 0!==d.id?c.N.Ba(d.id,c.H):e=!0;c.L.va&&c.J&&c.J("vss_network_hint")&&c.L.va(!0);f(g,h)};
this.ia(d.url,d.options);this.N.set(d,this.H).then(function(g){d.id=g;e&&c.N.Ba(d.id,c.H)}).catch(function(g){to(c,g)})}else this.ia(a,b)};
l.Ra=function(){var a=this;if(!po(this))throw Bm("throttleSend");this.i||(this.i=this.Y.R(function(){var b;return x(function(c){if(1==c.h)return v(c,a.N.ac("NEW",a.H),2);if(3!=c.h)return b=c.i,b?v(c,so(a,b),3):(a.Qb(),c.return());a.i&&(a.i=0,a.Ra());c.h=0})},this.uc))};
l.Qb=function(){this.Y.ea(this.i);this.i=0};
function so(a,b){var c,d;return x(function(e){switch(e.h){case 1:if(!po(a))throw c=Bm("immediateSend"),c;if(void 0===b.id){e.u(2);break}return v(e,a.N.Zc(b.id,a.H),3);case 3:(d=e.i)?b=d:a.Ca(Error("The request cannot be found in the database."));case 2:if(uo(a,b,a.mc)){e.u(4);break}a.Ca(Error("Networkless Logging: Stored logs request expired age limit"));if(void 0===b.id){e.u(5);break}return v(e,a.N.Ba(b.id,a.H),5);case 5:return e.return();case 4:b.skipRetry||(b=vo(a,b));if(!b){e.u(0);break}if(!b.skipRetry||
void 0===b.id){e.u(8);break}return v(e,a.N.Ba(b.id,a.H),8);case 8:a.ia(b.url,b.options,!!b.skipRetry),e.h=0}})}
function vo(a,b){if(!po(a))throw Bm("updateRequestHandlers");var c=b.options.onError?b.options.onError:function(){};
b.options.onError=function(e,f){var g,h,k;return x(function(m){switch(m.h){case 1:g=wo(f);if(!(a.J&&a.J("nwl_consider_error_code")&&g||a.J&&!a.J("nwl_consider_error_code")&&a.potentialEsfErrorCounter<=a.lb)){m.u(2);break}if(!a.L.ob){m.u(3);break}return v(m,a.L.ob(),3);case 3:if(a.L.U()){m.u(2);break}c(e,f);if(!a.J||!a.J("nwl_consider_error_code")||void 0===(null==(h=b)?void 0:h.id)){m.u(6);break}return v(m,a.N.Hb(b.id,a.H,!1),6);case 6:return m.return();case 2:if(a.J&&a.J("nwl_consider_error_code")&&
!g&&a.potentialEsfErrorCounter>a.lb)return m.return();a.potentialEsfErrorCounter++;if(void 0===(null==(k=b)?void 0:k.id)){m.u(8);break}return b.sendCount<a.oc?v(m,a.N.Hb(b.id,a.H),12):v(m,a.N.Ba(b.id,a.H),8);case 12:a.Y.R(function(){a.L.U()&&a.Ra()},a.nc);
case 8:c(e,f),m.h=0}})};
var d=b.options.onSuccess?b.options.onSuccess:function(){};
b.options.onSuccess=function(e,f){var g;return x(function(h){if(1==h.h)return void 0===(null==(g=b)?void 0:g.id)?h.u(2):v(h,a.N.Ba(b.id,a.H),2);a.L.va&&a.J&&a.J("vss_network_hint")&&a.L.va(!0);d(e,f);h.h=0})};
return b}
function uo(a,b,c){b=b.timestamp;return a.now()-b>=c?!1:!0}
function ro(a){if(!po(a))throw Bm("retryQueuedRequests");a.N.ac("QUEUED",a.H).then(function(b){b&&!uo(a,b,a.kc)?a.Y.R(function(){return x(function(c){if(1==c.h)return void 0===b.id?c.u(2):v(c,a.N.Hb(b.id,a.H),2);ro(a);c.h=0})}):a.L.U()&&a.Ra()})}
function to(a,b){a.zc&&!a.L.U()?a.zc(b):a.handleError(b)}
function po(a){return!!a.H||a.bb}
function wo(a){var b;return(a=null==a?void 0:null==(b=a.error)?void 0:b.code)&&400<=a&&599>=a?!1:!0}
;function xo(a,b){this.version=a;this.args=b}
;function yo(a,b){this.topic=a;this.h=b}
yo.prototype.toString=function(){return this.topic};var zo=B("ytPubsub2Pubsub2Instance")||new K;K.prototype.subscribe=K.prototype.subscribe;K.prototype.unsubscribeByKey=K.prototype.Ga;K.prototype.publish=K.prototype.ra;K.prototype.clear=K.prototype.clear;A("ytPubsub2Pubsub2Instance",zo);var Ao=B("ytPubsub2Pubsub2SubscribedKeys")||{};A("ytPubsub2Pubsub2SubscribedKeys",Ao);var Bo=B("ytPubsub2Pubsub2TopicToKeys")||{};A("ytPubsub2Pubsub2TopicToKeys",Bo);var Co=B("ytPubsub2Pubsub2IsAsync")||{};A("ytPubsub2Pubsub2IsAsync",Co);
A("ytPubsub2Pubsub2SkipSubKey",null);function Do(a,b){var c=Eo();c&&c.publish.call(c,a.toString(),a,b)}
function Fo(a){var b=Go,c=Eo();if(!c)return 0;var d=c.subscribe(b.toString(),function(e,f){var g=B("ytPubsub2Pubsub2SkipSubKey");g&&g==d||(g=function(){if(Ao[d])try{if(f&&b instanceof yo&&b!=e)try{var h=b.h,k=f;if(!k.args||!k.version)throw Error("yt.pubsub2.Data.deserialize(): serializedData is incomplete.");try{if(!h.oa){var m=new h;h.oa=m.version}var q=h.oa}catch(F){}if(!q||k.version!=q)throw Error("yt.pubsub2.Data.deserialize(): serializedData version is incompatible.");try{q=Reflect;var r=q.construct;
var w=k.args,t=w.length;if(0<t){var y=Array(t);for(k=0;k<t;k++)y[k]=w[k];var E=y}else E=[];f=r.call(q,h,E)}catch(F){throw F.message="yt.pubsub2.Data.deserialize(): "+F.message,F;}}catch(F){throw F.message="yt.pubsub2.pubsub2 cross-binary conversion error for "+b.toString()+": "+F.message,F;}a.call(window,f)}catch(F){Tk(F)}},Co[b.toString()]?B("yt.scheduler.instance")?bi.R(g):ml(g,0):g())});
Ao[d]=!0;Bo[b.toString()]||(Bo[b.toString()]=[]);Bo[b.toString()].push(d);return d}
function Ho(){var a=Io,b=Fo(function(c){a.apply(void 0,arguments);Jo(b)});
return b}
function Jo(a){var b=Eo();b&&("number"===typeof a&&(a=[a]),fb(a,function(c){b.unsubscribeByKey(c);delete Ao[c]}))}
function Eo(){return B("ytPubsub2Pubsub2Instance")}
;var Ko;
function Lo(){if(Ko)return Ko();var a={};Ko=Mn("LogsDatabaseV2",{Da:(a.LogsRequestsStore={Ia:2},a),pb:!1,upgrade:function(b,c,d){c(2)&&Qm(b,"LogsRequestsStore",{keyPath:"id",autoIncrement:!0});c(3);c(5)&&(d=d.objectStore("LogsRequestsStore"),d.h.indexNames.contains("newRequest")&&d.h.deleteIndex("newRequest"),Xm(d,"newRequestV2",["status","interface","timestamp"]));c(7)&&b.h.objectStoreNames.contains("sapisid")&&b.h.deleteObjectStore("sapisid");c(9)&&b.h.objectStoreNames.contains("SWHealthLog")&&b.h.deleteObjectStore("SWHealthLog")},
version:9});return Ko()}
;function Mo(a){return kn(Lo(),a)}
function No(a,b){var c,d,e,f;return x(function(g){if(1==g.h)return c={startTime:R(),transactionType:"YT_IDB_TRANSACTION_TYPE_WRITE"},v(g,Mo(b),2);if(3!=g.h)return d=g.i,e=Object.assign({},a,{options:JSON.parse(JSON.stringify(a.options)),interface:L("INNERTUBE_CONTEXT_CLIENT_NAME",0)}),v(g,Sm(d,"LogsRequestsStore",e),3);f=g.i;c.zd=R();Oo(c);return g.return(f)})}
function Po(a,b){var c,d,e,f,g,h,k;return x(function(m){if(1==m.h)return c={startTime:R(),transactionType:"YT_IDB_TRANSACTION_TYPE_READ"},v(m,Mo(b),2);if(3!=m.h)return d=m.i,e=L("INNERTUBE_CONTEXT_CLIENT_NAME",0),f=[a,e,0],g=[a,e,R()],h=IDBKeyRange.bound(f,g),k=void 0,v(m,Pm(d,["LogsRequestsStore"],{mode:"readwrite",T:!0},function(q){return bn(q.objectStore("LogsRequestsStore").index("newRequestV2"),{query:h,direction:"prev"},function(r){r.getValue()&&(k=r.getValue(),"NEW"===a&&(k.status="QUEUED",
r.update(k)))})}),3);
c.zd=R();Oo(c);return m.return(k)})}
function Qo(a,b){var c;return x(function(d){if(1==d.h)return v(d,Mo(b),2);c=d.i;return d.return(Pm(c,["LogsRequestsStore"],{mode:"readwrite",T:!0},function(e){var f=e.objectStore("LogsRequestsStore");return f.get(a).then(function(g){if(g)return g.status="QUEUED",Lm(f.h.put(g,void 0)).then(function(){return g})})}))})}
function Ro(a,b,c){c=void 0===c?!0:c;var d;return x(function(e){if(1==e.h)return v(e,Mo(b),2);d=e.i;return e.return(Pm(d,["LogsRequestsStore"],{mode:"readwrite",T:!0},function(f){var g=f.objectStore("LogsRequestsStore");return g.get(a).then(function(h){return h?(h.status="NEW",c&&(h.sendCount+=1),Lm(g.h.put(h,void 0)).then(function(){return h})):Fm.resolve(void 0)})}))})}
function So(a,b){var c;return x(function(d){if(1==d.h)return v(d,Mo(b),2);c=d.i;return d.return(c.delete("LogsRequestsStore",a))})}
function To(a){var b,c;return x(function(d){if(1==d.h)return v(d,Mo(a),2);b=d.i;c=R()-2592E6;return v(d,Pm(b,["LogsRequestsStore"],{mode:"readwrite",T:!0},function(e){return Zm(e.objectStore("LogsRequestsStore"),{},function(f){if(f.getValue().timestamp<=c)return f.delete().then(function(){return f.continue()})})}),0)})}
function Uo(){x(function(a){return v(a,In(),0)})}
function Oo(a){M("nwl_csi_killswitch")||.01>=Math.random()&&Do("nwl_transaction_latency_payload",a)}
;var Vo={},Wo=Mn("ServiceWorkerLogsDatabase",{Da:(Vo.SWHealthLog={Ia:1},Vo),pb:!0,upgrade:function(a,b){b(1)&&Xm(Qm(a,"SWHealthLog",{keyPath:"id",autoIncrement:!0}),"swHealthNewRequest",["interface","timestamp"])},
version:1});function Xo(a){return kn(Wo(),a)}
function Yo(a){var b,c;x(function(d){if(1==d.h)return v(d,Xo(a),2);b=d.i;c=R()-2592E6;return v(d,Pm(b,["SWHealthLog"],{mode:"readwrite",T:!0},function(e){return Zm(e.objectStore("SWHealthLog"),{},function(f){if(f.getValue().timestamp<=c)return f.delete().then(function(){return f.continue()})})}),0)})}
function Zo(a){var b;return x(function(c){if(1==c.h)return v(c,Xo(a),2);b=c.i;return v(c,b.clear("SWHealthLog"),0)})}
;var $o={},ap=0;function bp(a){var b=new Image,c=""+ap++;$o[c]=b;b.onload=b.onerror=function(){delete $o[c]};
b.src=a}
;function cp(){this.h=new Map;this.i=!1}
function dp(){if(!cp.h){var a=B("yt.networkRequestMonitor.instance")||new cp;A("yt.networkRequestMonitor.instance",a);cp.h=a}return cp.h}
cp.prototype.requestComplete=function(a,b){b&&(this.i=!0);a=this.removeParams(a);this.h.get(a)||this.h.set(a,b)};
cp.prototype.isEndpointCFR=function(a){a=this.removeParams(a);return(a=this.h.get(a))?!1:!1===a&&this.i?!0:null};
cp.prototype.removeParams=function(a){return a.split("?")[0]};
cp.prototype.removeParams=cp.prototype.removeParams;cp.prototype.isEndpointCFR=cp.prototype.isEndpointCFR;cp.prototype.requestComplete=cp.prototype.requestComplete;cp.getInstance=dp;var ep;function fp(){ep||(ep=new cm("yt.offline"));return ep}
function gp(a){if(M("offline_error_handling")){var b=fp().get("errors",!0)||{};b[a.message]={name:a.name,stack:a.stack};a.level&&(b[a.message].level=a.level);fp().set("errors",b,2592E3,!0)}}
;function hp(){ff.call(this);var a=this;this.j=!1;this.i=ai();this.i.ha("networkstatus-online",function(){if(a.j&&M("offline_error_handling")){var b=fp().get("errors",!0);if(b){for(var c in b)if(b[c]){var d=new P(c,"sent via offline_errors");d.name=b[c].name;d.stack=b[c].stack;d.level=b[c].level;Tk(d)}fp().set("errors",{},2592E3,!0)}}})}
u(hp,ff);function ip(){if(!hp.h){var a=B("yt.networkStatusManager.instance")||new hp;A("yt.networkStatusManager.instance",a);hp.h=a}return hp.h}
l=hp.prototype;l.U=function(){return this.i.U()};
l.va=function(a){this.i.i=a};
l.Sc=function(){var a=window.navigator.onLine;return void 0===a?!0:a};
l.Nc=function(){this.j=!0};
l.ha=function(a,b){return this.i.ha(a,b)};
l.ob=function(a){a=Zh(this.i,a);a.then(function(b){M("use_cfr_monitor")&&dp().requestComplete("generate_204",b)});
return a};
hp.prototype.sendNetworkCheckRequest=hp.prototype.ob;hp.prototype.listen=hp.prototype.ha;hp.prototype.enableErrorFlushing=hp.prototype.Nc;hp.prototype.getWindowStatus=hp.prototype.Sc;hp.prototype.networkStatusHint=hp.prototype.va;hp.prototype.isNetworkAvailable=hp.prototype.U;hp.getInstance=ip;function jp(a){a=void 0===a?{}:a;ff.call(this);var b=this;this.i=this.o=0;this.j=ip();var c=B("yt.networkStatusManager.instance.listen").bind(this.j);c&&(a.nb?(this.nb=a.nb,c("networkstatus-online",function(){kp(b,"publicytnetworkstatus-online")}),c("networkstatus-offline",function(){kp(b,"publicytnetworkstatus-offline")})):(c("networkstatus-online",function(){gf(b,"publicytnetworkstatus-online")}),c("networkstatus-offline",function(){gf(b,"publicytnetworkstatus-offline")})))}
u(jp,ff);jp.prototype.U=function(){var a=B("yt.networkStatusManager.instance.isNetworkAvailable");return a?a.bind(this.j)():!0};
jp.prototype.va=function(a){var b=B("yt.networkStatusManager.instance.networkStatusHint").bind(this.j);b&&b(a)};
jp.prototype.ob=function(a){var b=this,c;return x(function(d){c=B("yt.networkStatusManager.instance.sendNetworkCheckRequest").bind(b.j);return M("skip_network_check_if_cfr")&&dp().isEndpointCFR("generate_204")?d.return(new Promise(function(e){var f;b.va((null==(f=window.navigator)?void 0:f.onLine)||!0);e(b.U())})):c?d.return(c(a)):d.return(!0)})};
function kp(a,b){a.nb?a.i?(bi.ea(a.o),a.o=bi.R(function(){a.m!==b&&(gf(a,b),a.m=b,a.i=R())},a.nb-(R()-a.i))):(gf(a,b),a.m=b,a.i=R()):gf(a,b)}
;var lp;function mp(){var a=oo.call;lp||(lp=new jp({wr:!0,qr:!0}));a.call(oo,this,{N:{Jc:To,Ba:So,ac:Po,Zc:Qo,Hb:Ro,set:No},L:lp,handleError:Tk,Ca:Uk,ia:np,now:R,zc:gp,Y:Yl(),Db:"publicytnetworkstatus-online",Cb:"publicytnetworkstatus-offline",gb:!0,fb:.1,lb:Nk("potential_esf_error_limit",10),J:M,Ka:!(Zl()&&op())});this.j=new Gh;M("networkless_immediately_drop_all_requests")&&Uo();Jn("LogsDatabaseV2")}
u(mp,oo);function pp(){var a=B("yt.networklessRequestController.instance");a||(a=new mp,A("yt.networklessRequestController.instance",a),M("networkless_logging")&&yn().then(function(b){a.H=b;qo(a);a.j.resolve();a.gb&&Math.random()<=a.fb&&a.H&&Yo(a.H);M("networkless_immediately_drop_sw_health_store")&&qp(a)}));
return a}
mp.prototype.writeThenSend=function(a,b){b||(b={});Zl()||(this.h=!1);oo.prototype.writeThenSend.call(this,a,b)};
mp.prototype.sendThenWrite=function(a,b,c){b||(b={});Zl()||(this.h=!1);oo.prototype.sendThenWrite.call(this,a,b,c)};
mp.prototype.sendAndWrite=function(a,b){b||(b={});Zl()||(this.h=!1);oo.prototype.sendAndWrite.call(this,a,b)};
mp.prototype.awaitInitialization=function(){return this.j.promise};
function qp(a){var b;x(function(c){if(!a.H)throw b=Bm("clearSWHealthLogsDb"),b;return c.return(Zo(a.H).catch(function(d){a.handleError(d)}))})}
function np(a,b,c){M("use_cfr_monitor")&&Ep(a,b);if(M("use_request_time_ms_header"))b.headers&&(b.headers["X-Goog-Request-Time"]=JSON.stringify(Math.round(R())));else{var d;if(null==(d=b.postParams)?0:d.requestTimeMs)b.postParams.requestTimeMs=Math.round(R())}if(c&&0===Object.keys(b).length){var e=void 0===e?"":e;var f=void 0===f?!1:f;if(a)if(e)vl(a,void 0,"POST",e);else if(L("USE_NET_AJAX_FOR_PING_TRANSPORT",!1))vl(a,void 0,"GET","",void 0,void 0,f);else{b:{try{var g=new bb({url:a});if(g.j&&g.i||
g.l){var h=lc(mc(5,a)),k;if(!(k=!h||!h.endsWith("/aclk"))){var m=a.search(xc),q=wc(a,0,"ri",m);if(0>q)var r=null;else{var w=a.indexOf("&",q);if(0>w||w>m)w=m;r=decodeURIComponent(a.slice(q+3,-1!==w?w:0).replace(/\+/g," "))}k="1"!==r}var t=!k;break b}}catch(E){}t=!1}if(t){b:{try{if(window.navigator&&window.navigator.sendBeacon&&window.navigator.sendBeacon(a,"")){var y=!0;break b}}catch(E){}y=!1}c=y?!0:!1}else c=!1;c||bp(a)}}else sl(a,b)}
function Ep(a,b){var c=b.onError?b.onError:function(){};
b.onError=function(e,f){dp().requestComplete(a,!1);c(e,f)};
var d=b.onSuccess?b.onSuccess:function(){};
b.onSuccess=function(e,f){dp().requestComplete(a,!0);d(e,f)}}
function op(){return"www.youtube-nocookie.com"!==nc(document.location.toString())}
;var cq=!1,dq=z.ytNetworklessLoggingInitializationOptions||{isNwlInitialized:cq};A("ytNetworklessLoggingInitializationOptions",dq);function eq(){var a;x(function(b){if(1==b.h)return v(b,yn(),2);a=b.i;if(!a||!Zl()&&!M("nwl_init_require_datasync_id_killswitch")||!op())return b.u(0);cq=!0;dq.isNwlInitialized=cq;return v(b,pp().awaitInitialization(),0)})}
;function fq(a){var b=this;this.config_=null;a?this.config_=a:Wn()&&(this.config_=Xn());Tl(function(){mo(b)},5E3)}
fq.prototype.isReady=function(){!this.config_&&Wn()&&(this.config_=Xn());return!!this.config_};
function no(a,b,c,d){function e(y){y=void 0===y?!1:y;var E;if(d.retry&&"www.youtube-nocookie.com"!=h&&(y||M("skip_ls_gel_retry")||"application/json"!==g.headers["Content-Type"]||(E=ko(b,c,m,k)),E)){var F=g.onSuccess,O=g.onFetchSuccess;g.onSuccess=function(N,Q){lo(E);F(N,Q)};
c.onFetchSuccess=function(N,Q){lo(E);O(N,Q)}}try{y&&d.retry&&!d.hc.bypassNetworkless?(g.method="POST",d.hc.writeThenSend?pp().writeThenSend(t,g):pp().sendAndWrite(t,g)):M("web_all_payloads_via_jspb")?sl(t,g):(g.method="POST",g.postParams||(g.postParams={}),sl(t,g))}catch(N){if("InvalidAccessError"==N.name)E&&(lo(E),E=0),Uk(Error("An extension is blocking network request."));
else throw N;}E&&Tl(function(){mo(a)},5E3)}
!L("VISITOR_DATA")&&"visitor_id"!==b&&.01>Math.random()&&Uk(new P("Missing VISITOR_DATA when sending innertube request.",b,c,d));if(!a.isReady()){var f=new P("innertube xhrclient not ready",b,c,d);Tk(f);throw f;}var g={headers:d.headers||{},method:"POST",postParams:c,postBody:d.postBody,postBodyFormat:d.postBodyFormat||"JSON",onTimeout:function(){d.onTimeout()},
onFetchTimeout:d.onTimeout,onSuccess:function(y,E){if(d.onSuccess)d.onSuccess(E)},
onFetchSuccess:function(y){if(d.onSuccess)d.onSuccess(y)},
onError:function(y,E){if(d.onError)d.onError(E)},
onFetchError:function(y){if(d.onError)d.onError(y)},
timeout:d.timeout,withCredentials:!0};g.headers["Content-Type"]||(g.headers["Content-Type"]="application/json");var h="";(f=a.config_.Vc)&&(h=f);var k=a.config_.Xc||!1,m=go(k,h,d);Object.assign(g.headers,m);(f=g.headers.Authorization)&&!h&&(g.headers["x-origin"]=window.location.origin);var q="/youtubei/"+a.config_.innertubeApiVersion+"/"+b,r={alt:"json"},w=a.config_.Wc&&f;w=w&&f.startsWith("Bearer");w||(r.key=a.config_.innertubeApiKey);var t=fl(""+h+q,r||{},!0);(B("ytNetworklessLoggingInitializationOptions")?
dq.isNwlInitialized:cq)?wn().then(function(y){e(y)}):e(!1)}
;var gq=0,hq=Lc?"webkit":Kc?"moz":Ic?"ms":Hc?"o":"";A("ytDomDomGetNextId",B("ytDomDomGetNextId")||function(){return++gq});var iq={stopImmediatePropagation:1,stopPropagation:1,preventMouseEvent:1,preventManipulation:1,preventDefault:1,layerX:1,layerY:1,screenX:1,screenY:1,scale:1,rotation:1,webkitMovementX:1,webkitMovementY:1};
function jq(a){this.type="";this.state=this.source=this.data=this.currentTarget=this.relatedTarget=this.target=null;this.charCode=this.keyCode=0;this.metaKey=this.shiftKey=this.ctrlKey=this.altKey=!1;this.rotation=this.clientY=this.clientX=0;this.scale=1;this.changedTouches=this.touches=null;try{if(a=a||window.event){this.event=a;for(var b in a)b in iq||(this[b]=a[b]);this.scale=a.scale;this.rotation=a.rotation;var c=a.target||a.srcElement;c&&3==c.nodeType&&(c=c.parentNode);this.target=c;var d=a.relatedTarget;
if(d)try{d=d.nodeName?d:null}catch(e){d=null}else"mouseover"==this.type?d=a.fromElement:"mouseout"==this.type&&(d=a.toElement);this.relatedTarget=d;this.clientX=void 0!=a.clientX?a.clientX:a.pageX;this.clientY=void 0!=a.clientY?a.clientY:a.pageY;this.keyCode=a.keyCode?a.keyCode:a.which;this.charCode=a.charCode||("keypress"==this.type?this.keyCode:0);this.altKey=a.altKey;this.ctrlKey=a.ctrlKey;this.shiftKey=a.shiftKey;this.metaKey=a.metaKey;this.h=a.pageX;this.i=a.pageY}}catch(e){}}
function kq(a){if(document.body&&document.documentElement){var b=document.body.scrollTop+document.documentElement.scrollTop;a.h=a.clientX+(document.body.scrollLeft+document.documentElement.scrollLeft);a.i=a.clientY+b}}
jq.prototype.preventDefault=function(){this.event&&(this.event.returnValue=!1,this.event.preventDefault&&this.event.preventDefault())};
jq.prototype.stopPropagation=function(){this.event&&(this.event.cancelBubble=!0,this.event.stopPropagation&&this.event.stopPropagation())};
jq.prototype.stopImmediatePropagation=function(){this.event&&(this.event.cancelBubble=!0,this.event.stopImmediatePropagation&&this.event.stopImmediatePropagation())};var ob=z.ytEventsEventsListeners||{};A("ytEventsEventsListeners",ob);var lq=z.ytEventsEventsCounter||{count:0};A("ytEventsEventsCounter",lq);
function mq(a,b,c,d){d=void 0===d?{}:d;a.addEventListener&&("mouseenter"!=b||"onmouseenter"in document?"mouseleave"!=b||"onmouseenter"in document?"mousewheel"==b&&"MozBoxSizing"in document.documentElement.style&&(b="MozMousePixelScroll"):b="mouseout":b="mouseover");return nb(function(e){var f="boolean"===typeof e[4]&&e[4]==!!d,g=Qa(e[4])&&Qa(d)&&tb(e[4],d);return!!e.length&&e[0]==a&&e[1]==b&&e[2]==c&&(f||g)})}
var nq=db(function(){var a=!1;try{var b=Object.defineProperty({},"capture",{get:function(){a=!0}});
window.addEventListener("test",null,b)}catch(c){}return a});
function oq(a,b,c,d){d=void 0===d?{}:d;if(!a||!a.addEventListener&&!a.attachEvent)return"";var e=mq(a,b,c,d);if(e)return e;e=++lq.count+"";var f=!("mouseenter"!=b&&"mouseleave"!=b||!a.addEventListener||"onmouseenter"in document);var g=f?function(h){h=new jq(h);if(!qf(h.relatedTarget,function(k){return k==a}))return h.currentTarget=a,h.type=b,c.call(a,h)}:function(h){h=new jq(h);
h.currentTarget=a;return c.call(a,h)};
g=Sk(g);a.addEventListener?("mouseenter"==b&&f?b="mouseover":"mouseleave"==b&&f?b="mouseout":"mousewheel"==b&&"MozBoxSizing"in document.documentElement.style&&(b="MozMousePixelScroll"),nq()||"boolean"===typeof d?a.addEventListener(b,g,d):a.addEventListener(b,g,!!d.capture)):a.attachEvent("on"+b,g);ob[e]=[a,b,c,g,d];return e}
function pq(a){a&&("string"==typeof a&&(a=[a]),fb(a,function(b){if(b in ob){var c=ob[b],d=c[0],e=c[1],f=c[3];c=c[4];d.removeEventListener?nq()||"boolean"===typeof c?d.removeEventListener(e,f,c):d.removeEventListener(e,f,!!c.capture):d.detachEvent&&d.detachEvent("on"+e,f);delete ob[b]}}))}
;var qq=window.ytcsi&&window.ytcsi.now?window.ytcsi.now:window.performance&&window.performance.timing&&window.performance.now&&window.performance.timing.navigationStart?function(){return window.performance.timing.navigationStart+window.performance.now()}:function(){return(new Date).getTime()};function rq(a){this.F=a;this.i=null;this.m=0;this.s=null;this.o=0;this.j=[];for(a=0;4>a;a++)this.j.push(0);this.l=0;this.K=oq(window,"mousemove",Xa(this.O,this));a=Xa(this.I,this);"function"===typeof a&&(a=Sk(a));this.P=window.setInterval(a,25)}
Za(rq,J);rq.prototype.O=function(a){void 0===a.h&&kq(a);var b=a.h;void 0===a.i&&kq(a);this.i=new mf(b,a.i)};
rq.prototype.I=function(){if(this.i){var a=qq();if(0!=this.m){var b=this.s,c=this.i,d=b.x-c.x;b=b.y-c.y;d=Math.sqrt(d*d+b*b)/(a-this.m);this.j[this.l]=.5<Math.abs((d-this.o)/this.o)?1:0;for(c=b=0;4>c;c++)b+=this.j[c]||0;3<=b&&this.F();this.o=d}this.m=a;this.s=this.i;this.l=(this.l+1)%4}};
rq.prototype.C=function(){window.clearInterval(this.P);pq(this.K)};var sq={};
function tq(a){var b=void 0===a?{}:a;a=void 0===b.ed?!1:b.ed;b=void 0===b.Oc?!0:b.Oc;if(null==B("_lact",window)){var c=parseInt(L("LACT"),10);c=isFinite(c)?Date.now()-Math.max(c,0):-1;A("_lact",c,window);A("_fact",c,window);-1==c&&uq();oq(document,"keydown",uq);oq(document,"keyup",uq);oq(document,"mousedown",uq);oq(document,"mouseup",uq);a?oq(window,"touchmove",function(){vq("touchmove",200)},{passive:!0}):(oq(window,"resize",function(){vq("resize",200)}),b&&oq(window,"scroll",function(){vq("scroll",200)}));
new rq(function(){vq("mouse",100)});
oq(document,"touchstart",uq,{passive:!0});oq(document,"touchend",uq,{passive:!0})}}
function vq(a,b){sq[a]||(sq[a]=!0,bi.R(function(){uq();sq[a]=!1},b))}
function uq(){null==B("_lact",window)&&tq();var a=Date.now();A("_lact",a,window);-1==B("_fact",window)&&A("_fact",a,window);(a=B("ytglobal.ytUtilActivityCallback_"))&&a()}
function wq(){var a=B("_lact",window);return null==a?-1:Math.max(Date.now()-a,0)}
;var xq=z.ytPubsubPubsubInstance||new K,yq=z.ytPubsubPubsubSubscribedKeys||{},zq=z.ytPubsubPubsubTopicToKeys||{},Aq=z.ytPubsubPubsubIsSynchronous||{};function Bq(a,b){var c=Cq();if(c&&b){var d=c.subscribe(a,function(){var e=arguments;var f=function(){yq[d]&&b.apply&&"function"==typeof b.apply&&b.apply(window,e)};
try{Aq[a]?f():ml(f,0)}catch(g){Tk(g)}},void 0);
yq[d]=!0;zq[a]||(zq[a]=[]);zq[a].push(d);return d}return 0}
function Dq(a){var b=Cq();b&&("number"===typeof a?a=[a]:"string"===typeof a&&(a=[parseInt(a,10)]),fb(a,function(c){b.unsubscribeByKey(c);delete yq[c]}))}
function Eq(a,b){var c=Cq();c&&c.publish.apply(c,arguments)}
function Fq(a){var b=Cq();if(b)if(b.clear(a),a)Gq(a);else for(var c in zq)Gq(c)}
function Cq(){return z.ytPubsubPubsubInstance}
function Gq(a){zq[a]&&(a=zq[a],fb(a,function(b){yq[b]&&delete yq[b]}),a.length=0)}
K.prototype.subscribe=K.prototype.subscribe;K.prototype.unsubscribeByKey=K.prototype.Ga;K.prototype.publish=K.prototype.ra;K.prototype.clear=K.prototype.clear;A("ytPubsubPubsubInstance",xq);A("ytPubsubPubsubTopicToKeys",zq);A("ytPubsubPubsubIsSynchronous",Aq);A("ytPubsubPubsubSubscribedKeys",yq);var Hq=Symbol("injectionDeps");function Iq(a){this.name=a}
Iq.prototype.toString=function(){return"InjectionToken("+this.name+")"};
function Jq(){this.key=Kq}
function Lq(){this.h=new Map;this.i=new Map}
Lq.prototype.resolve=function(a){return a instanceof Jq?Mq(this,a.key,[],!0):Mq(this,a,[])};
function Mq(a,b,c,d){d=void 0===d?!1:d;if(-1<c.indexOf(b))throw Error("Deps cycle for: "+b);if(a.i.has(b))return a.i.get(b);if(!a.h.has(b)){if(d)return;throw Error("No provider for: "+b);}d=a.h.get(b);c.push(b);if(d.xc)var e=d.xc;else if(d.Cd)e=d[Hq]?Nq(a,d[Hq],c):[],e=d.Cd.apply(d,ia(e));else if(d.wc){e=d.wc;var f=e[Hq]?Nq(a,e[Hq],c):[];e=new (Function.prototype.bind.apply(e,[null].concat(ia(f))))}else throw Error("Could not resolve providers for: "+b);c.pop();d.Gr||a.i.set(b,e);return e}
function Nq(a,b,c){return b?b.map(function(d){return d instanceof Jq?Mq(a,d.key,c,!0):Mq(a,d,c)}):[]}
;var Oq;function Pq(){Oq||(Oq=new Lq);return Oq}
;function Qq(){this.store={};this.h={}}
Qq.prototype.storePayload=function(a,b){a=Rq(a);this.store[a]?this.store[a].push(b):(this.h={},this.store[a]=[b]);return a};
Qq.prototype.extractMatchingEntries=function(a){a=Sq(this,a);for(var b=[],c=0;c<a.length;c++)this.store[a[c]]&&(b.push.apply(b,ia(this.store[a[c]])),delete this.store[a[c]]);return b};
Qq.prototype.getSequenceCount=function(a){a=Sq(this,a);for(var b=0,c=0;c<a.length;c++)b+=this.store[a[c]].length||0;return b};
function Sq(a,b){var c=Rq(b);if(a.h[c])return a.h[c];var d=Object.keys(a.store)||[];if(1>=d.length&&Rq(b)===d[0])return d;for(var e=[],f=0;f<d.length;f++){var g=d[f].split("/");if(Tq(b.auth,g[0])){var h=b.isJspb;Tq(void 0===h?"undefined":h?"true":"false",g[1])&&Tq(b.cttAuthInfo,g[2])&&e.push(d[f])}}return a.h[c]=e}
function Tq(a,b){return void 0===a||"undefined"===a?!0:a===b}
Qq.prototype.getSequenceCount=Qq.prototype.getSequenceCount;Qq.prototype.extractMatchingEntries=Qq.prototype.extractMatchingEntries;Qq.prototype.storePayload=Qq.prototype.storePayload;function Rq(a){return[void 0===a.auth?"undefined":a.auth,void 0===a.isJspb?"undefined":a.isJspb,void 0===a.cttAuthInfo?"undefined":a.cttAuthInfo].join("/")}
;function Uq(a,b){if(a)return a[b.name]}
;var Vq=Nk("initial_gel_batch_timeout",2E3),Wq=Math.pow(2,16)-1,Xq=void 0;function Yq(){this.j=this.h=this.i=0}
var Zq=new Yq,$q=new Yq,ar,br=!0,cr=z.ytLoggingTransportGELQueue_||new Map;A("ytLoggingTransportGELQueue_",cr);var dr=z.ytLoggingTransportGELProtoQueue_||new Map;A("ytLoggingTransportGELProtoQueue_",dr);var er=z.ytLoggingTransportTokensToCttTargetIds_||{};A("ytLoggingTransportTokensToCttTargetIds_",er);var fr=z.ytLoggingTransportTokensToJspbCttTargetIds_||{};A("ytLoggingTransportTokensToJspbCttTargetIds_",fr);var gr={};function hr(){var a=B("yt.logging.ims");a||(a=new Qq,A("yt.logging.ims",a));return a}
function ir(a,b){M("web_all_payloads_via_jspb")&&Uk(new P("transport.log called for JSON in JSPB only experiment"));if("log_event"===a.endpoint){jr(a);var c=kr(a);if(M("use_new_in_memory_storage")){gr[c]=!0;var d={cttAuthInfo:c,isJspb:!1};hr().storePayload(d,a.payload);lr(b,[],c,!1,d)}else d=cr.get(c)||[],cr.set(c,d),d.push(a.payload),lr(b,d,c)}}
function mr(a,b){if("log_event"===a.endpoint){jr(void 0,a);var c=kr(a,!0);if(M("use_new_in_memory_storage")){gr[c]=!0;var d={cttAuthInfo:c,isJspb:!0};hr().storePayload(d,a.payload.toJSON());lr(b,[],c,!0,d)}else d=dr.get(c)||[],dr.set(c,d),a=a.payload.toJSON(),d.push(a),lr(b,d,c,!0)}}
function lr(a,b,c,d,e){d=void 0===d?!1:d;a&&(Xq=new a);a=Nk("tvhtml5_logging_max_batch")||Nk("web_logging_max_batch")||100;var f=R(),g=d?$q.j:Zq.j;b=b.length;e&&(b=hr().getSequenceCount(e));b>=a?ar||(ar=nr(function(){or({writeThenSend:!0},M("flush_only_full_queue")?c:void 0,d);ar=void 0},0)):10<=f-g&&(pr(d),d?$q.j=f:Zq.j=f)}
function qr(a,b){M("web_all_payloads_via_jspb")&&Uk(new P("transport.logIsolatedGelPayload called in JSPB only experiment"));if("log_event"===a.endpoint){jr(a);var c=kr(a),d=new Map;d.set(c,[a.payload]);b&&(Xq=new b);return new Cf(function(e,f){Xq&&Xq.isReady()?rr(d,Xq,e,f,{bypassNetworkless:!0},!0):e()})}}
function sr(a,b){if("log_event"===a.endpoint){jr(void 0,a);var c=kr(a,!0),d=new Map;d.set(c,[a.payload.toJSON()]);b&&(Xq=new b);return new Cf(function(e){Xq&&Xq.isReady()?tr(d,Xq,e,{bypassNetworkless:!0},!0):e()})}}
function kr(a,b){var c="";if(a.Ja)c="visitorOnlyApprovedKey";else if(a.cttAuthInfo){if(void 0===b?0:b){b=a.cttAuthInfo.token;c=a.cttAuthInfo;var d=new ck;c.videoId?d.setVideoId(c.videoId):c.playlistId&&Xd(d,2,xk,c.playlistId);fr[b]=d}else b=a.cttAuthInfo,c={},b.videoId?c.videoId=b.videoId:b.playlistId&&(c.playlistId=b.playlistId),er[a.cttAuthInfo.token]=c;c=a.cttAuthInfo.token}return c}
function or(a,b,c){a=void 0===a?{}:a;c=void 0===c?!1:c;!c&&M("web_all_payloads_via_jspb")&&Uk(new P("transport.flushLogs called for JSON in JSPB only experiment"));new Cf(function(d,e){c?(ur($q.i),ur($q.h),$q.h=0):(ur(Zq.i),ur(Zq.h),Zq.h=0);if(Xq&&Xq.isReady())if(M("use_new_in_memory_storage")){var f=a,g=c,h=Xq;f=void 0===f?{}:f;g=void 0===g?!1:g;var k=new Map,m=new Map;if(void 0!==b)g?(e=hr().extractMatchingEntries({isJspb:g,cttAuthInfo:b}),k.set(b,e),tr(k,h,d,f)):(k=hr().extractMatchingEntries({isJspb:g,
cttAuthInfo:b}),m.set(b,k),rr(m,h,d,e,f));else if(g){e=p(Object.keys(gr));for(g=e.next();!g.done;g=e.next())m=g.value,g=hr().extractMatchingEntries({isJspb:!0,cttAuthInfo:m}),0<g.length&&k.set(m,g),delete gr[m];tr(k,h,d,f)}else{k=p(Object.keys(gr));for(g=k.next();!g.done;g=k.next()){g=g.value;var q=hr().extractMatchingEntries({isJspb:!1,cttAuthInfo:g});0<q.length&&m.set(g,q);delete gr[g]}rr(m,h,d,e,f)}}else f=a,k=c,h=Xq,f=void 0===f?{}:f,k=void 0===k?!1:k,void 0!==b?k?(e=new Map,k=dr.get(b)||[],e.set(b,
k),tr(e,h,d,f),dr.delete(b)):(k=new Map,m=cr.get(b)||[],k.set(b,m),rr(k,h,d,e,f),cr.delete(b)):k?(tr(dr,h,d,f),dr.clear()):(rr(cr,h,d,e,f),cr.clear());else pr(c),d()})}
function pr(a){a=void 0===a?!1:a;if(M("web_gel_timeout_cap")&&(!a&&!Zq.h||a&&!$q.h)){var b=nr(function(){or({writeThenSend:!0},void 0,a)},6E4);
a?$q.h=b:Zq.h=b}ur(a?$q.i:Zq.i);b=L("LOGGING_BATCH_TIMEOUT",Nk("web_gel_debounce_ms",1E4));M("shorten_initial_gel_batch_timeout")&&br&&(b=Vq);b=nr(function(){or({writeThenSend:!0},void 0,a)},b);
a?$q.i=b:Zq.i=b}
function rr(a,b,c,d,e,f){e=void 0===e?{}:e;var g=Math.round(R()),h=a.size,k={};a=p(a);for(var m=a.next();!m.done;k={Ua:k.Ua,qa:k.qa,Ea:k.Ea,Wa:k.Wa,Va:k.Va},m=a.next()){var q=p(m.value);m=q.next().value;q=q.next().value;k.qa=vb({context:Yn(b.config_||Xn())});if(!Pa(q)&&!M("throw_err_when_logevent_malformed_killswitch")){d();break}k.qa.events=q;(q=er[m])&&vr(k.qa,m,q);delete er[m];k.Ea="visitorOnlyApprovedKey"===m;wr(k.qa,g,k.Ea);xr(e);k.Wa=function(r){M("update_log_event_config")&&bi.R(function(){return x(function(w){return v(w,
yr(r),0)})});
h--;h||c()};
k.Ua=0;k.Va=function(r){return function(){r.Ua++;if(e.bypassNetworkless&&1===r.Ua)try{no(b,"log_event",r.qa,zr({writeThenSend:!0},r.Ea,r.Wa,r.Va,f)),br=!1}catch(w){Tk(w),d()}h--;h||c()}}(k);
try{no(b,"log_event",k.qa,zr(e,k.Ea,k.Wa,k.Va,f)),br=!1}catch(r){Tk(r),d()}}}
function tr(a,b,c,d,e){d=void 0===d?{}:d;var f=Math.round(R()),g=a.size,h=new Map([].concat(ia(a)));h=p(h);for(var k=h.next();!k.done;k=h.next()){var m=p(k.value).next().value,q=a.get(m);k=new yk;var r=fo(b.config_||Xn());G(k,hj,1,r);q=q?Ar(q):[];q=p(q);for(r=q.next();!r.done;r=q.next())ee(k,3,Yj,r.value);(q=fr[m])&&Br(k,m,q);delete fr[m];m="visitorOnlyApprovedKey"===m;Cr(k,f,m);xr(d);k=me(k);m=zr(d,m,function(w){M("update_log_event_config")&&bi.R(function(){return x(function(t){return v(t,yr(w),
0)})});
g--;g||c()},function(){g--;
g||c()},e);
m.headers["Content-Type"]="application/json+protobuf";m.postBodyFormat="JSPB";m.postBody=k;no(b,"log_event","",m);br=!1}}
function xr(a){M("always_send_and_write")&&(a.writeThenSend=!1)}
function zr(a,b,c,d,e){a={retry:!0,onSuccess:c,onError:d,hc:a,Ja:b,kr:!!e,headers:{},postBodyFormat:"",postBody:""};Dr()&&(a.headers["X-Goog-Request-Time"]=JSON.stringify(Math.round(R())));return a}
function wr(a,b,c){Dr()||(a.requestTimeMs=String(b));M("unsplit_gel_payloads_in_logs")&&(a.unsplitGelPayloadsInLogs=!0);!c&&(b=L("EVENT_ID"))&&(c=Er(),a.serializedClientEventId={serializedEventId:b,clientCounter:String(c)})}
function Cr(a,b,c){Dr()||D(a,2,b);if(!c&&(b=L("EVENT_ID"))){c=Er();var d=new bk;D(d,1,b);D(d,2,c);G(a,bk,5,d)}}
function Er(){var a=L("BATCH_CLIENT_COUNTER")||0;a||(a=Math.floor(Math.random()*Wq/2));a++;a>Wq&&(a=1);Kk("BATCH_CLIENT_COUNTER",a);return a}
function vr(a,b,c){if(c.videoId)var d="VIDEO";else if(c.playlistId)d="PLAYLIST";else return;a.credentialTransferTokenTargetId=c;a.context=a.context||{};a.context.user=a.context.user||{};a.context.user.credentialTransferTokens=[{token:b,scope:d}]}
function Br(a,b,c){if(fe(c,1===Yd(c,xk)?1:-1))var d=1;else if(c.getPlaylistId())d=2;else return;G(a,ck,4,c);a=Zd(a,hj,1)||new hj;c=Zd(a,fj,3)||new fj;var e=new ej;D(e,2,b);D(e,1,d);ee(c,12,ej,e);G(a,fj,3,c)}
function Ar(a){for(var b=[],c=0;c<a.length;c++)try{b.push(new Yj(a[c]))}catch(d){Tk(new P("Transport failed to deserialize "+String(a[c])))}return b}
function jr(a,b){if(B("yt.logging.transport.enableScrapingForTest")){var c=B("yt.logging.transport.scrapedPayloadsForTesting"),d=B("yt.logging.transport.payloadToScrape","");b&&(b=B("yt.logging.transport.getScrapedPayloadFromClientEventsFunction").bind(b.payload)())&&c.push(b);a&&a.payload[d]&&c.push((null==a?void 0:a.payload)[d]);A("yt.logging.transport.scrapedPayloadsForTesting",c)}}
function Dr(){return M("use_request_time_ms_header")||M("lr_use_request_time_ms_header")}
function nr(a,b){return M("transport_use_scheduler")?Tl(a,b):ml(a,b)}
function ur(a){M("transport_use_scheduler")?bi.ea(a):window.clearTimeout(a)}
function yr(a){var b,c,d,e,f,g,h,k,m,q;return x(function(r){return 1==r.h?(d=null==(b=a)?void 0:null==(c=b.responseContext)?void 0:c.globalConfigGroup,e=Uq(d,Ri),g=null==(f=d)?void 0:f.hotHashData,h=Uq(d,Qi),m=null==(k=d)?void 0:k.coldHashData,q=Pq().resolve(Sn),e&&g?v(r,Tn(q,e,g),2):r.u(2)):h&&m?v(r,Un(q,h,m),0):r.u(0)})}
;var Fr=z.ytLoggingGelSequenceIdObj_||{};A("ytLoggingGelSequenceIdObj_",Fr);
function Gr(a,b,c,d){d=void 0===d?{}:d;var e={},f=Math.round(d.timestamp||R());e.eventTimeMs=f<Number.MAX_SAFE_INTEGER?f:0;e[a]=b;M("enable_unknown_lact_fix_on_html5")&&tq();a=wq();e.context={lastActivityMs:String(d.timestamp||!isFinite(a)?-1:a)};M("log_sequence_info_on_gel_web")&&d.Z&&(a=e.context,b=d.Z,b={index:Hr(b),groupKey:b},a.sequence=b,d.Xb&&delete Fr[d.Z]);(d.pc?qr:ir)({endpoint:"log_event",payload:e,cttAuthInfo:d.cttAuthInfo,Ja:d.Ja},c)}
function Ir(a){or(void 0,void 0,void 0===a?!1:a)}
function Hr(a){Fr[a]=a in Fr?Fr[a]+1:0;return Fr[a]}
;var Jr=[];var Kr=z.ytLoggingGelSequenceIdObj_||{};A("ytLoggingGelSequenceIdObj_",Kr);function Lr(a,b,c){c=void 0===c?{}:c;var d=Math.round(c.timestamp||R());D(a,1,d<Number.MAX_SAFE_INTEGER?d:0);var e=wq();d=new Xj;D(d,1,c.timestamp||!isFinite(e)?-1:e);if(M("log_sequence_info_on_gel_web")&&c.Z){e=c.Z;var f=Hr(e),g=new Wj;D(g,2,f);D(g,1,e);G(d,Wj,3,g);c.Xb&&delete Kr[c.Z]}G(a,Xj,33,d);(c.pc?sr:mr)({endpoint:"log_event",payload:a,cttAuthInfo:c.cttAuthInfo,Ja:c.Ja},b)}
;function Mr(a,b){b=void 0===b?{}:b;var c=!1;L("ytLoggingEventsDefaultDisabled",!1)&&(c=!0);Lr(a,c?null:fq,b)}
;function Nr(a,b){var c=new Yj;ce(c,Nj,72,Zj,a);Mr(c,b)}
function Or(a,b,c){var d=new Yj;ce(d,Mj,73,Zj,a);c?Lr(d,c,b):Mr(d,b)}
function Pr(a,b,c){var d=new Yj;ce(d,Lj,78,Zj,a);c?Lr(d,c,b):Mr(d,b)}
function Qr(a,b,c){var d=new Yj;ce(d,Oj,208,Zj,a);c?Lr(d,c,b):Mr(d,b)}
function Rr(a,b,c){var d=new Yj;ce(d,Ej,156,Zj,a);c?Lr(d,c,b):Mr(d,b)}
function Sr(a,b,c){var d=new Yj;ce(d,Ij,215,Zj,a);c?Lr(d,c,b):Mr(d,b)}
function Tr(a,b,c){var d=new Yj;ce(d,Dj,111,Zj,a);c?Lr(d,c,b):Mr(d,b)}
;function lm(a,b,c){c=void 0===c?{}:c;if(M("migrate_events_to_ts")){c=void 0===c?{}:c;var d=fq;L("ytLoggingEventsDefaultDisabled",!1)&&fq===fq&&(d=null);M("web_all_payloads_via_jspb")?Jr.push({Eb:a,payload:b,options:c}):Gr(a,b,d,c)}else d=fq,L("ytLoggingEventsDefaultDisabled",!1)&&fq==fq&&(d=null),Gr(a,b,d,c)}
;var Ur=[{Bb:function(a){return"Cannot read property '"+a.key+"'"},
kb:{Error:[{regexp:/(Permission denied) to access property "([^']+)"/,groups:["reason","key"]}],TypeError:[{regexp:/Cannot read property '([^']+)' of (null|undefined)/,groups:["key","value"]},{regexp:/\u65e0\u6cd5\u83b7\u53d6\u672a\u5b9a\u4e49\u6216 (null|undefined) \u5f15\u7528\u7684\u5c5e\u6027\u201c([^\u201d]+)\u201d/,groups:["value","key"]},{regexp:/\uc815\uc758\ub418\uc9c0 \uc54a\uc74c \ub610\ub294 (null|undefined) \ucc38\uc870\uc778 '([^']+)' \uc18d\uc131\uc744 \uac00\uc838\uc62c \uc218 \uc5c6\uc2b5\ub2c8\ub2e4./,
groups:["value","key"]},{regexp:/No se puede obtener la propiedad '([^']+)' de referencia nula o sin definir/,groups:["key"]},{regexp:/Unable to get property '([^']+)' of (undefined or null) reference/,groups:["key","value"]},{regexp:/(null) is not an object \(evaluating '(?:([^.]+)\.)?([^']+)'\)/,groups:["value","base","key"]}]}},{Bb:function(a){return"Cannot call '"+a.key+"'"},
kb:{TypeError:[{regexp:/(?:([^ ]+)?\.)?([^ ]+) is not a function/,groups:["base","key"]},{regexp:/([^ ]+) called on (null or undefined)/,groups:["key","value"]},{regexp:/Object (.*) has no method '([^ ]+)'/,groups:["base","key"]},{regexp:/Object doesn't support property or method '([^ ]+)'/,groups:["key"]},{regexp:/\u30aa\u30d6\u30b8\u30a7\u30af\u30c8\u306f '([^']+)' \u30d7\u30ed\u30d1\u30c6\u30a3\u307e\u305f\u306f\u30e1\u30bd\u30c3\u30c9\u3092\u30b5\u30dd\u30fc\u30c8\u3057\u3066\u3044\u307e\u305b\u3093/,
groups:["key"]},{regexp:/\uac1c\uccb4\uac00 '([^']+)' \uc18d\uc131\uc774\ub098 \uba54\uc11c\ub4dc\ub97c \uc9c0\uc6d0\ud558\uc9c0 \uc54a\uc2b5\ub2c8\ub2e4./,groups:["key"]}]}},{Bb:function(a){return a.key+" is not defined"},
kb:{ReferenceError:[{regexp:/(.*) is not defined/,groups:["key"]},{regexp:/Can't find variable: (.*)/,groups:["key"]}]}}];var Wr={na:[],ka:[{callback:Vr,weight:500}]};function Vr(a){if("JavaException"===a.name)return!0;a=a.stack;return a.includes("chrome://")||a.includes("chrome-extension://")||a.includes("moz-extension://")}
;function Xr(){this.ka=[];this.na=[]}
var Yr;function Zr(){if(!Yr){var a=Yr=new Xr;a.na.length=0;a.ka.length=0;Wr.na&&a.na.push.apply(a.na,Wr.na);Wr.ka&&a.ka.push.apply(a.ka,Wr.ka)}return Yr}
;var $r=new K;function as(a){function b(){return a.charCodeAt(d++)}
var c=a.length,d=0;do{var e=bs(b);if(Infinity===e)break;var f=e>>3;switch(e&7){case 0:e=bs(b);if(2===f)return e;break;case 1:if(2===f)return;d+=8;break;case 2:e=bs(b);if(2===f)return a.substr(d,e);d+=e;break;case 5:if(2===f)return;d+=4;break;default:return}}while(d<c)}
function bs(a){var b=a(),c=b&127;if(128>b)return c;b=a();c|=(b&127)<<7;if(128>b)return c;b=a();c|=(b&127)<<14;if(128>b)return c;b=a();return 128>b?c|(b&127)<<21:Infinity}
;function cs(a,b,c,d){if(a)if(Array.isArray(a)){var e=d;for(d=0;d<a.length&&!(a[d]&&(e+=ds(d,a[d],b,c),500<e));d++);d=e}else if("object"===typeof a)for(e in a){if(a[e]){var f=a[e];var g=b;var h=c;g="string"!==typeof f||"clickTrackingParams"!==e&&"trackingParams"!==e?0:(f=as(atob(f.replace(/-/g,"+").replace(/_/g,"/"))))?ds(e+".ve",f,g,h):0;d+=g;d+=ds(e,a[e],b,c);if(500<d)break}}else c[b]=es(a),d+=c[b].length;else c[b]=es(a),d+=c[b].length;return d}
function ds(a,b,c,d){c+="."+a;a=es(b);d[c]=a;return c.length+a.length}
function es(a){try{return("string"===typeof a?a:String(JSON.stringify(a))).substr(0,500)}catch(b){return"unable to serialize "+typeof a+" ("+b.message+")"}}
;var fs=new Set,gs=0,hs=0,is=0,js=[],ks=["PhantomJS","Googlebot","TO STOP THIS SECURITY SCAN go/scan"];function km(a){ls(a)}
function ms(a){ls(a,"WARNING")}
function ls(a,b,c,d,e,f){f=void 0===f?{}:f;f.name=c||L("INNERTUBE_CONTEXT_CLIENT_NAME",1);f.version=d||L("INNERTUBE_CONTEXT_CLIENT_VERSION");var g=f||{},h=void 0===b?"ERROR":b;h=void 0===h?"ERROR":h;if(a){a.hasOwnProperty("level")&&a.level&&(h=a.level);if(M("console_log_js_exceptions")){var k=[];k.push("Name: "+a.name);k.push("Message: "+a.message);a.hasOwnProperty("params")&&k.push("Error Params: "+JSON.stringify(a.params));a.hasOwnProperty("args")&&k.push("Error args: "+JSON.stringify(a.args));
k.push("File name: "+a.fileName);k.push("Stacktrace: "+a.stack);window.console.log(k.join("\n"),a)}if(!(5<=gs)){var m=js,q=ze(a),r=q.message||"Unknown Error",w=q.name||"UnknownError",t=q.stack||a.i||"Not available";if(t.startsWith(w+": "+r)){var y=t.split("\n");y.shift();t=y.join("\n")}var E=q.lineNumber||"Not available",F=q.fileName||"Not available",O=t,N=0;if(a.hasOwnProperty("args")&&a.args&&a.args.length)for(var Q=0;Q<a.args.length&&!(N=cs(a.args[Q],"params."+Q,g,N),500<=N);Q++);else if(a.hasOwnProperty("params")&&
a.params){var da=a.params;if("object"===typeof a.params)for(var U in da){if(da[U]){var qb="params."+U,Wa=es(da[U]);g[qb]=Wa;N+=qb.length+Wa.length;if(500<N)break}}else g.params=es(da)}if(m.length)for(var oa=0;oa<m.length&&!(N=cs(m[oa],"params.context."+oa,g,N),500<=N);oa++);navigator.vendor&&!g.hasOwnProperty("vendor")&&(g["device.vendor"]=navigator.vendor);var I={message:r,name:w,lineNumber:E,fileName:F,stack:O,params:g,sampleWeight:1},ma=Number(a.columnNumber);isNaN(ma)||(I.lineNumber=I.lineNumber+
":"+ma);if("IGNORED"===a.level)var fa=0;else a:{for(var Ce=Zr(),De=p(Ce.na),rd=De.next();!rd.done;rd=De.next()){var qa=rd.value;if(I.message&&I.message.match(qa.xr)){fa=qa.weight;break a}}for(var rp=p(Ce.ka),dk=rp.next();!dk.done;dk=rp.next()){var sp=dk.value;if(sp.callback(I)){fa=sp.weight;break a}}fa=1}I.sampleWeight=fa;for(var tp=p(Ur),ek=tp.next();!ek.done;ek=tp.next()){var fk=ek.value;if(fk.kb[I.name])for(var up=p(fk.kb[I.name]),gk=up.next();!gk.done;gk=up.next()){var vp=gk.value,Pg=I.message.match(vp.regexp);
if(Pg){I.params["params.error.original"]=Pg[0];for(var hk=vp.groups,wp={},sd=0;sd<hk.length;sd++)wp[hk[sd]]=Pg[sd+1],I.params["params.error."+hk[sd]]=Pg[sd+1];I.message=fk.Bb(wp);break}}}I.params||(I.params={});var xp=Zr();I.params["params.errorServiceSignature"]="msg="+xp.na.length+"&cb="+xp.ka.length;I.params["params.serviceWorker"]="false";z.document&&z.document.querySelectorAll&&(I.params["params.fscripts"]=String(document.querySelectorAll("script:not([nonce])").length));Cb("sample").constructor!==
Ab&&(I.params["params.fconst"]="true");window.yterr&&"function"===typeof window.yterr&&window.yterr(I);if(0!==I.sampleWeight&&!fs.has(I.message)){if("ERROR"===h){$r.ra("handleError",I);if(M("record_app_crashed_web")&&0===is&&1===I.sampleWeight)if(is++,M("errors_via_jspb")){var ik=new Aj;D(ik,1,1);if(!M("report_client_error_with_app_crash_ks")){var yp=new vj;D(yp,1,I.message);var zp=new wj;G(zp,vj,3,yp);var Ap=new xj;G(Ap,wj,5,zp);var Bp=new yj;G(Bp,xj,9,Ap);G(ik,yj,4,Bp)}var Cp=new Yj;ce(Cp,Aj,20,
Zj,ik);Mr(Cp)}else{var Dp={appCrashType:"APP_CRASH_TYPE_BREAKPAD"};M("report_client_error_with_app_crash_ks")||(Dp.systemHealth={crashData:{clientError:{logMessage:{message:I.message}}}});lm("appCrashed",Dp)}hs++}else"WARNING"===h&&$r.ra("handleWarning",I);if(M("kevlar_gel_error_routing"))a:{var Ee=h;if(M("errors_via_jspb")){if(ns())var Fp=void 0;else{var td=new sj;D(td,1,I.stack);I.fileName&&D(td,4,I.fileName);var Jb=I.lineNumber&&I.lineNumber.split?I.lineNumber.split(":"):[];0!==Jb.length&&(1!==
Jb.length||isNaN(Number(Jb[0]))?2!==Jb.length||isNaN(Number(Jb[0]))||isNaN(Number(Jb[1]))||(D(td,2,Number(Jb[0])),D(td,3,Number(Jb[1]))):D(td,2,Number(Jb[0])));var zc=new vj;D(zc,1,I.message);D(zc,3,I.name);D(zc,6,I.sampleWeight);"ERROR"===Ee?D(zc,2,2):"WARNING"===Ee?D(zc,2,1):D(zc,2,0);var jk=new tj;D(jk,1,!0);ce(jk,sj,3,uj,td);var $b=new pj;D($b,3,window.location.href);for(var Gp=L("FEXP_EXPERIMENTS",[]),kk=0;kk<Gp.length;kk++){var aw=Gp[kk];Hd($b);Wd($b,5,2,!1,!1).push(aw)}var lk=L("LATEST_ECATCHER_SERVICE_TRACKING_PARAMS");
if(!Lk()&&lk)for(var Hp=p(Object.keys(lk)),Ac=Hp.next();!Ac.done;Ac=Hp.next()){var Ip=Ac.value,mk=new rj;D(mk,1,Ip);D(mk,2,String(lk[Ip]));ee($b,4,rj,mk)}var nk=I.params;if(nk){var Jp=p(Object.keys(nk));for(Ac=Jp.next();!Ac.done;Ac=Jp.next()){var Kp=Ac.value,ok=new rj;D(ok,1,"client."+Kp);D(ok,2,String(nk[Kp]));ee($b,4,rj,ok)}}var Lp=L("SERVER_NAME"),Mp=L("SERVER_VERSION");if(Lp&&Mp){var pk=new rj;D(pk,1,"server.name");D(pk,2,Lp);ee($b,4,rj,pk);var qk=new rj;D(qk,1,"server.version");D(qk,2,Mp);ee($b,
4,rj,qk)}var Qg=new wj;G(Qg,pj,1,$b);G(Qg,tj,2,jk);G(Qg,vj,3,zc);Fp=Qg}var Np=Fp;if(!Np)break a;var Op=new Yj;ce(Op,wj,163,Zj,Np);Mr(Op)}else{if(ns())var Pp=void 0;else{var Fe={stackTrace:I.stack};I.fileName&&(Fe.filename=I.fileName);var Kb=I.lineNumber&&I.lineNumber.split?I.lineNumber.split(":"):[];0!==Kb.length&&(1!==Kb.length||isNaN(Number(Kb[0]))?2!==Kb.length||isNaN(Number(Kb[0]))||isNaN(Number(Kb[1]))||(Fe.lineNumber=Number(Kb[0]),Fe.columnNumber=Number(Kb[1])):Fe.lineNumber=Number(Kb[0]));
var rk={level:"ERROR_LEVEL_UNKNOWN",message:I.message,errorClassName:I.name,sampleWeight:I.sampleWeight};"ERROR"===Ee?rk.level="ERROR_LEVEL_ERROR":"WARNING"===Ee&&(rk.level="ERROR_LEVEL_WARNNING");var bw={isObfuscated:!0,browserStackInfo:Fe},ud={pageUrl:window.location.href,kvPairs:[]};L("FEXP_EXPERIMENTS")&&(ud.experimentIds=L("FEXP_EXPERIMENTS"));var sk=L("LATEST_ECATCHER_SERVICE_TRACKING_PARAMS");if(!Lk()&&sk)for(var Qp=p(Object.keys(sk)),Bc=Qp.next();!Bc.done;Bc=Qp.next()){var Rp=Bc.value;ud.kvPairs.push({key:Rp,
value:String(sk[Rp])})}var tk=I.params;if(tk){var Sp=p(Object.keys(tk));for(Bc=Sp.next();!Bc.done;Bc=Sp.next()){var Tp=Bc.value;ud.kvPairs.push({key:"client."+Tp,value:String(tk[Tp])})}}var Up=L("SERVER_NAME"),Vp=L("SERVER_VERSION");Up&&Vp&&(ud.kvPairs.push({key:"server.name",value:Up}),ud.kvPairs.push({key:"server.version",value:Vp}));Pp={errorMetadata:ud,stackTrace:bw,logMessage:rk}}var Wp=Pp;if(!Wp)break a;lm("clientError",Wp)}if("ERROR"===Ee||M("errors_flush_gel_always_killswitch"))b:if(M("migrate_events_to_ts"))c:{if(M("web_fp_via_jspb")&&
(Ir(!0),!M("web_fp_via_jspb_and_json")))break c;Ir()}else{if(M("web_fp_via_jspb")&&(Ir(!0),!M("web_fp_via_jspb_and_json")))break b;Ir()}}if(!M("suppress_error_204_logging")){var Ge=I.params||{},ac={urlParams:{a:"logerror",t:"jserror",type:I.name,msg:I.message.substr(0,250),line:I.lineNumber,level:h,"client.name":Ge.name},postParams:{url:L("PAGE_NAME",window.location.href),file:I.fileName},method:"POST"};Ge.version&&(ac["client.version"]=Ge.version);if(ac.postParams){I.stack&&(ac.postParams.stack=
I.stack);for(var Xp=p(Object.keys(Ge)),uk=Xp.next();!uk.done;uk=Xp.next()){var Yp=uk.value;ac.postParams["client."+Yp]=Ge[Yp]}var vk=L("LATEST_ECATCHER_SERVICE_TRACKING_PARAMS");if(vk)for(var Zp=p(Object.keys(vk)),wk=Zp.next();!wk.done;wk=Zp.next()){var $p=wk.value;ac.postParams[$p]=vk[$p]}var aq=L("SERVER_NAME"),bq=L("SERVER_VERSION");aq&&bq&&(ac.postParams["server.name"]=aq,ac.postParams["server.version"]=bq)}sl(L("ECATCHER_REPORT_HOST","")+"/error_204",ac)}try{fs.add(I.message)}catch(Dx){}gs++}}}}
function ns(){for(var a=p(ks),b=a.next();!b.done;b=a.next())if(bm(b.value.toLowerCase()))return!0;return!1}
function os(a){var b=Ja.apply(1,arguments);a.args||(a.args=[]);a.args.push.apply(a.args,ia(b))}
;function ps(){this.register=new Map}
function qs(a){a=p(a.register.values());for(var b=a.next();!b.done;b=a.next())b.value.Br("ABORTED")}
ps.prototype.clear=function(){qs(this);this.register.clear()};
var rs=new ps;var ss=Date.now().toString();
function ts(){a:{if(window.crypto&&window.crypto.getRandomValues)try{var a=Array(16),b=new Uint8Array(16);window.crypto.getRandomValues(b);for(var c=0;c<a.length;c++)a[c]=b[c];var d=a;break a}catch(e){}d=Array(16);for(a=0;16>a;a++){b=Date.now();for(c=0;c<b%23;c++)d[a]=Math.random();d[a]=Math.floor(256*Math.random())}if(ss)for(a=1,b=0;b<ss.length;b++)d[a%16]=d[a%16]^d[(a-1)%16]/4^ss.charCodeAt(b),a++}a=[];for(b=0;b<d.length;b++)a.push("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_".charAt(d[b]&63));
return a.join("")}
;var us=z.ytLoggingDocDocumentNonce_;us||(us=ts(),A("ytLoggingDocDocumentNonce_",us));var vs=us;var ws={Zh:0,te:1,Ee:2,Yl:3,bi:4,zq:5,Om:6,Do:7,Tn:8,ro:9,0:"DEFAULT",1:"CHAT",2:"CONVERSATIONS",3:"MINIPLAYER",4:"DIALOG",5:"VOZ",6:"MUSIC_WATCH_TABS",7:"SHARE",8:"PUSH_NOTIFICATIONS",9:"RICH_GRID_WATCH"};function xs(a){this.G=a}
function ys(a){return new xs({trackingParams:a})}
xs.prototype.getAsJson=function(){var a={};void 0!==this.G.trackingParams?a.trackingParams=this.G.trackingParams:(a.veType=this.G.veType,void 0!==this.G.veCounter&&(a.veCounter=this.G.veCounter),void 0!==this.G.elementIndex&&(a.elementIndex=this.G.elementIndex));void 0!==this.G.dataElement&&(a.dataElement=this.G.dataElement.getAsJson());void 0!==this.G.youtubeData&&(a.youtubeData=this.G.youtubeData);return a};
xs.prototype.getAsJspb=function(){var a=new Cj;if(void 0!==this.G.trackingParams){var b=this.G.trackingParams;if(null!=b)if("string"===typeof b)b=b?new jd(b,gd):kd();else if(b.constructor!==jd)if(fd(b))b=b.length?new jd(new Uint8Array(b),gd):kd();else throw Error();D(a,1,b)}else void 0!==this.G.veType&&D(a,2,this.G.veType),void 0!==this.G.veCounter&&D(a,6,this.G.veCounter),void 0!==this.G.elementIndex&&D(a,3,this.G.elementIndex);void 0!==this.G.dataElement&&(b=this.G.dataElement.getAsJspb(),G(a,Cj,
7,b));void 0!==this.G.youtubeData&&G(a,Si,8,this.G.jspbYoutubeData);return a};
xs.prototype.toString=function(){return JSON.stringify(this.getAsJson())};
xs.prototype.isClientVe=function(){return!this.G.trackingParams&&!!this.G.veType};function zs(a){a=void 0===a?0:a;return 0===a?"client-screen-nonce":"client-screen-nonce."+a}
function As(a){a=void 0===a?0:a;return 0===a?"ROOT_VE_TYPE":"ROOT_VE_TYPE."+a}
function Bs(a){return L(As(void 0===a?0:a))}
A("yt_logging_screen.getRootVeType",Bs);function Cs(a){return(a=Bs(void 0===a?0:a))?new xs({veType:a,youtubeData:void 0,jspbYoutubeData:void 0}):null}
function Ds(){var a=L("csn-to-ctt-auth-info");a||(a={},Kk("csn-to-ctt-auth-info",a));return a}
function Es(a){a=L(zs(void 0===a?0:a));if(!a&&!L("USE_CSN_FALLBACK",!0))return null;a||(a="UNDEFINED_CSN");return a?a:null}
A("yt_logging_screen.getCurrentCsn",Es);function Fs(a,b,c){var d=Ds();(c=Es(c))&&delete d[c];b&&(d[a]=b)}
function Gs(a){return Ds()[a]}
A("yt_logging_screen.getCttAuthInfo",Gs);
function Hs(a,b,c,d){c=void 0===c?0:c;if(a!==L(zs(c))||b!==L(As(c)))if(Fs(a,d,c),Kk(zs(c),a),Kk(As(c),b),b=function(){setTimeout(function(){if(a)if(M("web_time_via_jspb")){var e=new Dj;D(e,1,vs);D(e,2,a);M("use_default_heartbeat_client")?Tr(e):Tr(e,void 0,fq)}else e={clientDocumentNonce:vs,clientScreenNonce:a},M("use_default_heartbeat_client")?lm("foregroundHeartbeatScreenAssociated",e):Gr("foregroundHeartbeatScreenAssociated",e,fq)},0)},"requestAnimationFrame"in window)try{window.requestAnimationFrame(b)}catch(e){b()}else b()}
A("yt_logging_screen.setCurrentScreen",Hs);var Is=window.yt&&window.yt.msgs_||window.ytcfg&&window.ytcfg.msgs||{};A("yt.msgs_",Is);function Js(a){Fk(Is,arguments)}
;var Ks={se:3611,Dd:27686,Ed:85013,Fd:23462,Hd:157557,Id:42016,Jd:62407,Kd:26926,Gd:43781,Ld:51236,Md:79148,Nd:50160,Od:77504,ae:153587,be:87907,ce:18630,de:54445,ee:80935,ge:152172,he:105675,ie:150723,je:37521,ke:147285,le:47786,me:98349,ne:123695,oe:6827,pe:29434,qe:7282,re:124448,we:32276,ue:76278,xe:147868,ye:147869,ze:93911,Ae:106531,Be:27259,Ce:27262,De:27263,Fe:21759,Ge:27107,He:62936,Ie:160866,Je:49568,Ke:160789,Le:38408,Me:80637,Ne:68727,Oe:68728,Pe:80353,Qe:80356,Re:74610,Se:45707,Te:83962,
Ue:83970,Ve:46713,We:89711,Xe:74612,Ye:155792,Ze:93265,af:74611,bf:131380,df:128979,ef:139311,ff:128978,cf:131391,gf:105350,jf:139312,kf:134800,hf:131392,mf:113533,nf:93252,pf:99357,rf:94521,sf:114252,tf:113532,uf:94522,qf:94583,vf:88E3,wf:139580,xf:93253,yf:93254,zf:94387,Af:94388,Bf:93255,Cf:97424,lf:72502,Df:110111,Ef:76019,Gf:117092,Hf:117093,Ff:89431,If:110466,Jf:77240,Kf:60508,Lf:148123,Mf:148124,Nf:137401,Of:137402,Pf:137046,Qf:73393,Rf:113534,Sf:92098,Tf:131381,Uf:84517,Vf:83759,Wf:162711,
Xf:162712,Yf:80357,Zf:86113,ag:72598,cg:72733,dg:107349,eg:124275,fg:118203,gg:133275,hg:160157,ig:152569,jg:156651,kg:133274,lg:160159,mg:160158,ng:133272,og:133273,pg:133276,qg:144507,rg:143247,sg:156652,tg:143248,ug:143249,vg:143250,wg:143251,xg:156653,yg:144401,Ag:117431,zg:133797,Bg:153964,Cg:128572,Dg:133405,Eg:117429,Fg:117430,Gg:117432,Hg:120080,Ig:117259,Jg:156655,Kg:156654,Lg:121692,Mg:145656,Ng:156656,Og:145655,Pg:145653,Qg:145654,Rg:145657,Sg:132972,Tg:133051,Ug:133658,Vg:132971,Wg:97615,
Yg:143359,Xg:143356,ah:143361,Zg:143358,dh:143360,bh:143357,eh:142303,fh:143353,gh:143354,hh:144479,ih:143355,jh:31402,lh:133624,mh:146477,nh:133623,oh:133622,kh:133621,ph:84774,rh:160801,qh:95117,sh:150497,th:98930,uh:98931,vh:98932,wh:153320,xh:153321,yh:43347,zh:129889,Ah:149123,Bh:45474,Ch:100352,Dh:84758,Eh:98443,Fh:117985,Gh:74613,Hh:155911,Ih:74614,Jh:64502,Kh:136032,Lh:74615,Mh:74616,Nh:122224,Oh:74617,Ph:77820,Qh:74618,Rh:93278,Sh:93274,Th:93275,Uh:93276,Vh:22110,Wh:29433,Xh:133798,Yh:132295,
ai:120541,ci:82047,di:113550,fi:75836,gi:75837,hi:42352,ii:84512,ji:76065,ki:75989,ri:51879,si:16623,ti:32594,vi:27240,wi:32633,xi:74858,yi:156999,Ai:3945,zi:16989,Bi:45520,Ci:25488,Di:25492,Ei:25494,Fi:55760,Gi:14057,Hi:18451,Ii:57204,Ji:57203,Ki:17897,Li:57205,Mi:18198,Ni:17898,Oi:17909,Pi:43980,Qi:46220,Ri:11721,Si:147994,Ti:49954,Ui:96369,Vi:3854,Wi:151633,Xi:56251,Yi:159108,Zi:25624,aj:152036,rj:16906,sj:99999,tj:68172,uj:27068,vj:47973,wj:72773,xj:26970,yj:26971,zj:96805,Aj:17752,Bj:73233,Cj:109512,
Dj:22256,Ej:14115,Fj:22696,Gj:89278,Hj:89277,Ij:109513,Jj:43278,Kj:43459,Lj:43464,Mj:89279,Nj:43717,Oj:55764,Pj:22255,Qj:147912,Rj:89281,Sj:40963,Tj:43277,Uj:43442,Vj:91824,Wj:120137,Xj:96367,Yj:36850,Zj:72694,ak:37414,bk:36851,dk:124863,ck:121343,ek:73491,fk:54473,gk:166861,hk:43375,ik:46674,jk:143815,kk:139095,lk:144402,mk:149968,nk:149969,pk:32473,qk:72901,rk:72906,sk:50947,tk:50612,uk:50613,vk:50942,wk:84938,xk:84943,yk:84939,zk:84941,Ak:84944,Bk:84940,Ck:84942,Dk:35585,Ek:51926,Fk:79983,Gk:63238,
Hk:18921,Ik:63241,Jk:57893,Kk:41182,Lk:135732,Mk:33424,Nk:22207,Ok:42993,Pk:36229,Qk:22206,Rk:22205,Sk:18993,Tk:19001,Uk:18990,Vk:18991,Wk:18997,Xk:18725,Yk:19003,Zk:36874,al:44763,bl:33427,dl:67793,fl:22182,il:37091,jl:34650,kl:50617,ll:47261,ml:22287,nl:25144,ol:97917,pl:62397,ql:150871,rl:150874,sl:125598,ul:137935,vl:36961,wl:108035,xl:27426,yl:27857,zl:27846,Al:27854,Bl:69692,Cl:61411,Dl:39299,El:38696,Fl:62520,Gl:36382,Hl:108701,Il:50663,Jl:36387,Kl:14908,Ll:37533,Ml:105443,Nl:61635,Ol:62274,
Pl:161670,Ql:133818,Rl:65702,Sl:65703,Tl:65701,Ul:76256,Vl:166382,Wl:37671,Xl:49953,Zl:36216,am:28237,bm:39553,cm:29222,dm:26107,em:38050,fm:26108,hm:120745,gm:26109,im:26110,jm:66881,km:28236,lm:14586,mm:160598,nm:57929,om:74723,pm:44098,qm:44099,tm:23528,um:61699,rm:134104,sm:134103,vm:59149,wm:101951,xm:97346,ym:118051,zm:95102,Am:64882,Bm:119505,Cm:63595,Dm:63349,Em:95101,Fm:75240,Gm:27039,Hm:68823,Im:21537,Jm:83464,Km:75707,Lm:83113,Mm:101952,Nm:101953,Pm:79610,Qm:125755,Rm:24402,Sm:24400,Tm:32925,
Vm:57173,Um:156421,Wm:122502,Xm:145268,Ym:138480,Zm:64423,an:64424,bn:33986,cn:100828,dn:129089,en:21409,jn:135155,kn:135156,ln:135157,mn:135158,nn:158225,pn:135159,qn:135160,rn:135161,tn:135162,un:135163,sn:158226,vn:158227,wn:135164,xn:135165,yn:135166,fn:11070,gn:11074,hn:17880,zn:14001,Bn:30709,Cn:30707,Dn:30711,En:30710,Fn:30708,An:26984,Gn:146143,Hn:63648,In:63649,Jn:111059,Kn:5754,Ln:20445,Mn:151308,Nn:151152,Pn:130975,On:130976,Qn:110386,Rn:113746,Sn:66557,Un:17310,Vn:28631,Wn:21589,Xn:164817,
Yn:154946,Zn:68012,ao:162617,bo:60480,co:138664,eo:141121,fo:164502,ho:31571,jo:141978,ko:150105,lo:150106,mo:150107,no:150108,oo:76980,po:41577,qo:45469,so:38669,to:13768,uo:13777,vo:141842,wo:62985,xo:4724,yo:59369,zo:43927,Ao:43928,Bo:12924,Co:100355,Fo:56219,Go:27669,Ho:10337,Eo:47896,Io:122629,Ko:139723,Jo:139722,Lo:121258,Mo:107598,No:127991,Oo:96639,Po:107536,Qo:130169,Ro:96661,So:145188,To:96658,Uo:116646,Vo:159428,Wo:121122,Xo:96660,Yo:127738,Zo:127083,ap:155281,bp:162959,cp:163566,ep:147842,
fp:104443,gp:96659,hp:147595,ip:106442,jp:162776,kp:134840,lp:63667,mp:63668,np:63669,qp:130686,rp:147036,sp:78314,tp:147799,up:148649,vp:55761,wp:127098,xp:134841,yp:96368,zp:67374,Ap:48992,Bp:146176,Cp:49956,Dp:31961,Ep:26388,Fp:23811,Gp:5E4,Hp:126250,Ip:96370,Jp:47355,Kp:47356,Lp:37935,Mp:45521,Np:21760,Op:83769,Pp:49977,Qp:49974,Rp:93497,Sp:93498,Tp:34325,Up:140759,Vp:115803,Wp:123707,Xp:100081,Yp:35309,Zp:68314,aq:25602,bq:100339,cq:143516,fq:59018,gq:18248,hq:50625,iq:9729,jq:37168,kq:37169,
lq:21667,mq:16749,nq:18635,oq:39305,pq:18046,qq:53969,rq:8213,sq:93926,tq:102852,uq:110099,wq:22678,xq:69076,yq:137575,Aq:139224,Bq:100856,Cq:154430,Dq:17736,Eq:3832,Fq:147111,Gq:55759,Hq:64031,Nq:93044,Oq:93045,Pq:34388,Qq:17657,Rq:17655,Sq:39579,Tq:39578,Uq:77448,Vq:8196,Wq:11357,Xq:69877,Yq:8197,Zq:156512,br:161613,dr:156509,er:161612,fr:161614,gr:82039};function Ls(){var a=ub(Ms),b;return(new Cf(function(c,d){a.onSuccess=function(e){ll(e)?c(new Ns(e)):d(new Os("Request failed, status="+(e&&"status"in e?e.status:-1),"net.badstatus",e))};
a.onError=function(e){d(new Os("Unknown request error","net.unknown",e))};
a.onTimeout=function(e){d(new Os("Request timed out","net.timeout",e))};
b=sl("//googleads.g.doubleclick.net/pagead/id",a)})).qb(function(c){c instanceof Jf&&b.abort();
return Hf(c)})}
function Os(a,b,c){ab.call(this,a+", errorCode="+b);this.errorCode=b;this.xhr=c;this.name="PromiseAjaxError"}
u(Os,ab);function Ns(a){this.xhr=a}
;function Ps(){this.h=0;this.ba=null}
Ps.prototype.then=function(a,b,c){return 1===this.h&&a?(a=a.call(c,this.ba))&&"function"===typeof a.then?a:Qs(a):2===this.h&&b?(a=b.call(c,this.ba))&&"function"===typeof a.then?a:Rs(a):this};
Ps.prototype.getValue=function(){return this.ba};
Ps.prototype.$goog_Thenable=!0;function Rs(a){var b=new Ps;a=void 0===a?null:a;b.h=2;b.ba=void 0===a?null:a;return b}
function Qs(a){var b=new Ps;a=void 0===a?null:a;b.h=1;b.ba=void 0===a?null:a;return b}
;function Ss(a,b){var c=void 0===c?{}:c;a={method:void 0===b?"POST":b,mode:gl(a)?"same-origin":"cors",credentials:gl(a)?"same-origin":"include"};b={};for(var d=p(Object.keys(c)),e=d.next();!e.done;e=d.next())e=e.value,c[e]&&(b[e]=c[e]);0<Object.keys(b).length&&(a.headers=b);return a}
;function Ts(){return xg()||am&&bm("applewebkit")&&!bm("version")&&(!bm("safari")||bm("gsa/"))||Mc&&bm("version/")?!0:L("EOM_VISITOR_DATA")?!1:!0}
;function Us(a){ab.call(this,a.message||a.description||a.name);this.isMissing=a instanceof Vs;this.isTimeout=a instanceof Os&&"net.timeout"==a.errorCode;this.isCanceled=a instanceof Jf}
u(Us,ab);Us.prototype.name="BiscottiError";function Vs(){ab.call(this,"Biscotti ID is missing from server")}
u(Vs,ab);Vs.prototype.name="BiscottiMissingError";var Ms={format:"RAW",method:"GET",timeout:5E3,withCredentials:!0},Ws=null;function Xs(){if(M("disable_biscotti_fetch_entirely_for_all_web_clients"))return Error("Biscotti id fetching has been disabled entirely.");if(!Ts())return Error("User has not consented - not fetching biscotti id.");var a=L("PLAYER_VARS",{});if("1"==sb(a))return Error("Biscotti ID is not available in private embed mode");if(Vn(a))return Error("Biscotti id fetching has been disabled for pfl.")}
function Wk(){var a=Xs();if(void 0!==a)return Hf(a);Ws||(Ws=Ls().then(Ys).qb(function(b){return Zs(2,b)}));
return Ws}
function Ys(a){a=a.xhr.responseText;if(0!=a.lastIndexOf(")]}'",0))throw new Vs;a=JSON.parse(a.substr(4));if(1<(a.type||1))throw new Vs;a=a.id;Xk(a);Ws=Qs(a);$s(18E5,2);return a}
function Zs(a,b){b=new Us(b);Xk("");Ws=Rs(b);0<a&&$s(12E4,a-1);throw b;}
function $s(a,b){ml(function(){Ls().then(Ys,function(c){return Zs(b,c)}).qb(cb)},a)}
function at(){try{var a=B("yt.ads.biscotti.getId_");return a?a():Wk()}catch(b){return Hf(b)}}
;function bt(a){if("1"!=sb(L("PLAYER_VARS",{}))){a&&Vk();try{at().then(function(){},function(){}),ml(bt,18E5)}catch(b){Tk(b)}}}
;function ct(){this.wd=!0}
function dt(a){var b={},c=zg([]);c&&(b.Authorization=c,c=a=null==a?void 0:a.sessionIndex,void 0===c&&(c=Number(L("SESSION_INDEX",0)),c=isNaN(c)?0:c),M("voice_search_auth_header_removal")||(b["X-Goog-AuthUser"]=c),"INNERTUBE_HOST_OVERRIDE"in Jk||(b["X-Origin"]=window.location.origin),void 0===a&&"DELEGATED_SESSION_ID"in Jk&&(b["X-Goog-PageId"]=L("DELEGATED_SESSION_ID")));return b}
;var et={identityType:"UNAUTHENTICATED_IDENTITY_TYPE_UNKNOWN"};var ft=new Map([["dark","USER_INTERFACE_THEME_DARK"],["light","USER_INTERFACE_THEME_LIGHT"]]);function gt(){var a=void 0===a?window.location.href:a;if(M("kevlar_disable_theme_param"))return null;lc(mc(5,a));try{var b=el(a).theme;return ft.get(b)||null}catch(c){}return null}
;function ht(){this.h={};if(this.i=El()){var a=vg.get("CONSISTENCY",void 0);a&&jt(this,{encryptedTokenJarContents:a})}}
ht.prototype.handleResponse=function(a,b){if(!b)throw Error("request needs to be passed into ConsistencyService");var c,d;b=(null==(c=b.ga.context)?void 0:null==(d=c.request)?void 0:d.consistencyTokenJars)||[];var e;if(a=null==(e=a.responseContext)?void 0:e.consistencyTokenJar){e=p(b);for(c=e.next();!c.done;c=e.next())delete this.h[c.value.encryptedTokenJarContents];jt(this,a)}};
function jt(a,b){if(b.encryptedTokenJarContents&&(a.h[b.encryptedTokenJarContents]=b,"string"===typeof b.expirationSeconds)){var c=Number(b.expirationSeconds);setTimeout(function(){delete a.h[b.encryptedTokenJarContents]},1E3*c);
a.i&&Cl("CONSISTENCY",b.encryptedTokenJarContents,c,void 0,!0)}}
;var kt=window.location.hostname.split(".").slice(-2).join(".");function lt(){var a=L("LOCATION_PLAYABILITY_TOKEN");"TVHTML5"===L("INNERTUBE_CLIENT_NAME")&&(this.h=mt(this))&&(a=this.h.get("yt-location-playability-token"));a&&(this.locationPlayabilityToken=a,this.i=void 0)}
var nt;lt.getInstance=function(){nt=B("yt.clientLocationService.instance");nt||(nt=new lt,A("yt.clientLocationService.instance",nt));return nt};
l=lt.prototype;l.setLocationOnInnerTubeContext=function(a){a.client||(a.client={});this.i?(a.client.locationInfo||(a.client.locationInfo={}),a.client.locationInfo.latitudeE7=Math.floor(1E7*this.i.coords.latitude),a.client.locationInfo.longitudeE7=Math.floor(1E7*this.i.coords.longitude),a.client.locationInfo.horizontalAccuracyMeters=Math.round(this.i.coords.accuracy),a.client.locationInfo.forceLocationPlayabilityTokenRefresh=!0):this.locationPlayabilityToken&&(a.client.locationPlayabilityToken=this.locationPlayabilityToken)};
l.handleResponse=function(a){var b;a=null==(b=a.responseContext)?void 0:b.locationPlayabilityToken;void 0!==a&&(this.locationPlayabilityToken=a,this.i=void 0,"TVHTML5"===L("INNERTUBE_CLIENT_NAME")?(this.h=mt(this))&&this.h.set("yt-location-playability-token",a,15552E3):Cl("YT_CL",JSON.stringify({loctok:a}),15552E3,kt,!0))};
function mt(a){return void 0===a.h?new cm("yt-client-location"):a.h}
l.clearLocationPlayabilityToken=function(a){"TVHTML5"===a?(this.h=mt(this))&&this.h.remove("yt-location-playability-token"):Dl("YT_CL")};
l.getCurrentPositionFromGeolocation=function(){var a=this;if(!(navigator&&navigator.geolocation&&navigator.geolocation.getCurrentPosition))return Promise.reject(Error("Geolocation unsupported"));var b=!1,c=1E4;"MWEB"===L("INNERTUBE_CLIENT_NAME")&&(b=!0,c=15E3);return new Promise(function(d,e){navigator.geolocation.getCurrentPosition(function(f){a.i=f;d(f)},function(f){e(f)},{enableHighAccuracy:b,
maximumAge:0,timeout:c})})};
l.createUnpluggedLocationInfo=function(a){var b={};a=a.coords;if(null==a?0:a.latitude)b.latitudeE7=Math.floor(1E7*a.latitude);if(null==a?0:a.longitude)b.longitudeE7=Math.floor(1E7*a.longitude);if(null==a?0:a.accuracy)b.locationRadiusMeters=Math.round(a.accuracy);return b};function ot(a,b){var c,d=null==(c=Uq(a,lj))?void 0:c.signal;if(d&&b.Qa&&(c=b.Qa[d]))return c();var e;if((c=null==(e=Uq(a,jj))?void 0:e.request)&&b.Lc&&(e=b.Lc[c]))return e();for(var f in a)if(b.Tb[f]&&(a=b.Tb[f]))return a()}
;function pt(a){return function(){return new a}}
;var qt={},rt=(qt.WEB_UNPLUGGED="^unplugged/",qt.WEB_UNPLUGGED_ONBOARDING="^unplugged/",qt.WEB_UNPLUGGED_OPS="^unplugged/",qt.WEB_UNPLUGGED_PUBLIC="^unplugged/",qt.WEB_CREATOR="^creator/",qt.WEB_KIDS="^kids/",qt.WEB_EXPERIMENTS="^experiments/",qt.WEB_MUSIC="^music/",qt.WEB_REMIX="^music/",qt.WEB_MUSIC_EMBEDDED_PLAYER="^music/",qt.WEB_MUSIC_EMBEDDED_PLAYER="^main_app/|^sfv/",qt);
function st(a){var b=void 0===b?"UNKNOWN_INTERFACE":b;if(1===a.length)return a[0];var c=rt[b];if(c){var d=new RegExp(c),e=p(a);for(c=e.next();!c.done;c=e.next())if(c=c.value,d.exec(c))return c}var f=[];Object.entries(rt).forEach(function(g){var h=p(g);g=h.next().value;h=h.next().value;b!==g&&f.push(h)});
d=new RegExp(f.join("|"));a.sort(function(g,h){return g.length-h.length});
e=p(a);for(c=e.next();!c.done;c=e.next())if(c=c.value,!d.exec(c))return c;return a[0]}
;function tt(){}
tt.prototype.m=function(a,b,c){b=void 0===b?{}:b;c=void 0===c?et:c;var d=a.clickTrackingParams,e=this.l,f=!1;f=void 0===f?!1:f;e=void 0===e?!1:e;var g=L("INNERTUBE_CONTEXT");if(g){g=vb(g);M("web_no_tracking_params_in_shell_killswitch")||delete g.clickTracking;g.client||(g.client={});var h=g.client;"MWEB"===h.clientName&&(h.clientFormFactor=L("IS_TABLET")?"LARGE_FORM_FACTOR":"SMALL_FORM_FACTOR");h.screenWidthPoints=window.innerWidth;h.screenHeightPoints=window.innerHeight;h.screenPixelDensity=Math.round(window.devicePixelRatio||
1);h.screenDensityFloat=window.devicePixelRatio||1;h.utcOffsetMinutes=-Math.floor((new Date).getTimezoneOffset());var k=void 0===k?!1:k;Gl.getInstance();var m="USER_INTERFACE_THEME_LIGHT";Jl(165)?m="USER_INTERFACE_THEME_DARK":Jl(174)?m="USER_INTERFACE_THEME_LIGHT":!M("kevlar_legacy_browsers")&&window.matchMedia&&window.matchMedia("(prefers-color-scheme)").matches&&window.matchMedia("(prefers-color-scheme: dark)").matches&&(m="USER_INTERFACE_THEME_DARK");k=k?m:gt()||m;h.userInterfaceTheme=k;if(!f){if(k=
Ql())h.connectionType=k;M("web_log_effective_connection_type")&&(k=Rl())&&(g.client.effectiveConnectionType=k)}var q;if(M("web_log_memory_total_kbytes")&&(null==(q=z.navigator)?0:q.deviceMemory)){var r;q=null==(r=z.navigator)?void 0:r.deviceMemory;g.client.memoryTotalKbytes=""+1E6*q}r=el(z.location.href);!M("web_populate_internal_geo_killswitch")&&r.internalcountrycode&&(h.internalGeo=r.internalcountrycode);"MWEB"===h.clientName||"WEB"===h.clientName?(h.mainAppWebInfo={graftUrl:z.location.href},M("kevlar_woffle")&&
Al.h&&(r=Al.h,h.mainAppWebInfo.pwaInstallabilityStatus=!r.h&&r.i?"PWA_INSTALLABILITY_STATUS_CAN_BE_INSTALLED":"PWA_INSTALLABILITY_STATUS_UNKNOWN"),h.mainAppWebInfo.webDisplayMode=Bl(),h.mainAppWebInfo.isWebNativeShareAvailable=navigator&&void 0!==navigator.share):"TVHTML5"===h.clientName&&(!M("web_lr_app_quality_killswitch")&&(r=L("LIVING_ROOM_APP_QUALITY"))&&(h.tvAppInfo=Object.assign(h.tvAppInfo||{},{appQuality:r})),r=L("LIVING_ROOM_CERTIFICATION_SCOPE"))&&(h.tvAppInfo=Object.assign(h.tvAppInfo||
{},{certificationScope:r}));if(!M("web_populate_time_zone_itc_killswitch")){b:{if("undefined"!==typeof Intl)try{var w=(new Intl.DateTimeFormat).resolvedOptions().timeZone;break b}catch(qb){}w=void 0}w&&(h.timeZone=w)}(w=Ok())?h.experimentsToken=w:delete h.experimentsToken;w=Pk();ht.h||(ht.h=new ht);h=ht.h.h;r=[];q=0;for(var t in h)r[q++]=h[t];g.request=Object.assign({},g.request,{internalExperimentFlags:w,consistencyTokenJars:r});!M("web_prequest_context_killswitch")&&(t=L("INNERTUBE_CONTEXT_PREQUEST_CONTEXT"))&&
(g.request.externalPrequestContext=t);w=Gl.getInstance();t=Jl(58);w=w.get("gsml","");g.user=Object.assign({},g.user);t&&(g.user.enableSafetyMode=t);w&&(g.user.lockedSafetyMode=!0);M("warm_op_csn_cleanup")?e&&(f=Es())&&(g.clientScreenNonce=f):!f&&(f=Es())&&(g.clientScreenNonce=f);d&&(g.clickTracking={clickTrackingParams:d});if(d=B("yt.mdx.remote.remoteClient_"))g.remoteClient=d;lt.getInstance().setLocationOnInnerTubeContext(g);try{var y=hl(),E=y.bid;delete y.bid;g.adSignalsInfo={params:[],bid:E};var F=
p(Object.entries(y));for(var O=F.next();!O.done;O=F.next()){var N=p(O.value),Q=N.next().value,da=N.next().value;y=Q;E=da;d=void 0;null==(d=g.adSignalsInfo.params)||d.push({key:y,value:""+E})}}catch(qb){ls(qb)}F=g}else ls(Error("Error: No InnerTubeContext shell provided in ytconfig.")),F={};F={context:F};if(O=this.h(a)){this.i(F,O,b);var U;b="/youtubei/v1/"+st(this.j());(O=null==(U=Uq(a.commandMetadata,kj))?void 0:U.apiUrl)&&(b=O);U=b;(b=L("INNERTUBE_HOST_OVERRIDE"))&&(U=String(b)+String(oc(U)));b=
{};b.key=L("INNERTUBE_API_KEY");M("json_condensed_response")&&(b.prettyPrint="false");U=fl(U,b||{},!1);a=Object.assign({},{command:a},void 0);a={input:U,xa:Ss(U),ga:F,config:a};a.config.ab?a.config.ab.identity=c:a.config.ab={identity:c};return a}ls(new P("Error: Failed to create Request from Command.",a))};
ea.Object.defineProperties(tt.prototype,{l:{configurable:!0,enumerable:!0,get:function(){return!1}}});function ut(){}
u(ut,tt);ut.prototype.m=function(){return{input:"/getDatasyncIdsEndpoint",xa:Ss("/getDatasyncIdsEndpoint","GET"),ga:{}}};
ut.prototype.j=function(){return[]};
ut.prototype.h=function(){};
ut.prototype.i=function(){};var vt={},wt=(vt.GET_DATASYNC_IDS=pt(ut),vt);function xt(a){var b=Ja.apply(1,arguments);if(!zt(a)||b.some(function(d){return!zt(d)}))throw Error("Only objects may be merged.");
b=p(b);for(var c=b.next();!c.done;c=b.next())At(a,c.value);return a}
function At(a,b){for(var c in b)if(zt(b[c])){if(c in a&&!zt(a[c]))throw Error("Cannot merge an object into a non-object.");c in a||(a[c]={});At(a[c],b[c])}else if(Bt(b[c])){if(c in a&&!Bt(a[c]))throw Error("Cannot merge an array into a non-array.");c in a||(a[c]=[]);Ct(a[c],b[c])}else a[c]=b[c];return a}
function Ct(a,b){b=p(b);for(var c=b.next();!c.done;c=b.next())c=c.value,zt(c)?a.push(At({},c)):Bt(c)?a.push(Ct([],c)):a.push(c);return a}
function zt(a){return"object"===typeof a&&!Array.isArray(a)}
function Bt(a){return"object"===typeof a&&Array.isArray(a)}
;function Dt(a,b){xo.call(this,1,arguments);this.timer=b}
u(Dt,xo);var Et=new yo("aft-recorded",Dt);var Ft=window;function Gt(){this.timing={};this.clearResourceTimings=function(){};
this.webkitClearResourceTimings=function(){};
this.mozClearResourceTimings=function(){};
this.msClearResourceTimings=function(){};
this.oClearResourceTimings=function(){}}
var S=Ft.performance||Ft.mozPerformance||Ft.msPerformance||Ft.webkitPerformance||new Gt;var Ht=!1,It={'script[name="scheduler/scheduler"]':"sj",'script[name="player/base"]':"pj",'link[rel="stylesheet"][name="www-player"]':"pc",'link[rel="stylesheet"][name="player/www-player"]':"pc",'script[name="desktop_polymer/desktop_polymer"]':"dpj",'link[rel="import"][name="desktop_polymer"]':"dph",'script[name="mobile-c3"]':"mcj",'link[rel="stylesheet"][name="mobile-c3"]':"mcc",'script[name="player-plasma-ias-phone/base"]':"mcppj",'script[name="player-plasma-ias-tablet/base"]':"mcptj",'link[rel="stylesheet"][name="mobile-polymer-player-ias"]':"mcpc",
'link[rel="stylesheet"][name="mobile-polymer-player-svg-ias"]':"mcpsc",'script[name="mobile_blazer_core_mod"]':"mbcj",'link[rel="stylesheet"][name="mobile_blazer_css"]':"mbc",'script[name="mobile_blazer_logged_in_users_mod"]':"mbliuj",'script[name="mobile_blazer_logged_out_users_mod"]':"mblouj",'script[name="mobile_blazer_noncore_mod"]':"mbnj","#player_css":"mbpc",'script[name="mobile_blazer_desktopplayer_mod"]':"mbpj",'link[rel="stylesheet"][name="mobile_blazer_tablet_css"]':"mbtc",'script[name="mobile_blazer_watch_mod"]':"mbwj"};
Xa(S.clearResourceTimings||S.webkitClearResourceTimings||S.mozClearResourceTimings||S.msClearResourceTimings||S.oClearResourceTimings||cb,S);function Jt(a){var b=Kt("aft",a);if(b)return b;b=L((a||"")+"TIMING_AFT_KEYS",["ol"]);for(var c=b.length,d=0;d<c;d++){var e=Kt(b[d],a);if(e)return e}return NaN}
function Lt(){var a;if(M("csi_use_performance_navigation_timing")||M("csi_use_performance_navigation_timing_tvhtml5")){var b,c,d,e=null==S?void 0:null==(a=S.getEntriesByType)?void 0:null==(b=a.call(S,"navigation"))?void 0:null==(c=b[0])?void 0:null==(d=c.toJSON)?void 0:d.call(c);e?(e.requestStart=Mt(e.requestStart),e.responseEnd=Mt(e.responseEnd),e.redirectStart=Mt(e.redirectStart),e.redirectEnd=Mt(e.redirectEnd),e.domainLookupEnd=Mt(e.domainLookupEnd),e.connectStart=Mt(e.connectStart),e.connectEnd=
Mt(e.connectEnd),e.responseStart=Mt(e.responseStart),e.secureConnectionStart=Mt(e.secureConnectionStart),e.domainLookupStart=Mt(e.domainLookupStart),e.isPerformanceNavigationTiming=!0,a=e):a=S.timing}else a=S.timing;return a}
function Nt(){return(M("csi_use_time_origin")||M("csi_use_time_origin_tvhtml5"))&&S.timeOrigin?Math.floor(S.timeOrigin):S.timing.navigationStart}
function Mt(a){return Math.round(Nt()+a)}
function Ot(a){var b;(b=B("ytcsi."+(a||"")+"data_"))||(b={tick:{},info:{}},A("ytcsi."+(a||"")+"data_",b));return b}
function Pt(a){a=Ot(a);a.info||(a.info={});return a.info}
function Qt(a){a=Ot(a);a.metadata||(a.metadata={});return a.metadata}
function Rt(a){a=Ot(a);a.tick||(a.tick={});return a.tick}
function Kt(a,b){if(a=Rt(b)[a])return"number"===typeof a?a:a[a.length-1]}
function St(a){a=Ot(a);if(a.gel){var b=a.gel;b.gelInfos||(b.gelInfos={});b.gelTicks||(b.gelTicks={})}else a.gel={gelTicks:{},gelInfos:{}};return a.gel}
function Tt(a){a=St(a);a.gelInfos||(a.gelInfos={});return a.gelInfos}
function Ut(a){var b=Ot(a).nonce;b||(b=ts(),Ot(a).nonce=b);return b}
function Vt(a){var b=Kt("_start",a),c=Jt(a);b&&c&&!Ht&&(Do(Et,new Dt(Math.round(c-b),a)),Ht=!0)}
function Wt(a,b){for(var c=p(Object.keys(b)),d=c.next();!d.done;d=c.next())if(d=d.value,!Object.keys(a).includes(d)||"object"===typeof b[d]&&!Wt(a[d],b[d]))return!1;return!0}
;function Xt(){if(S.getEntriesByType){var a=S.getEntriesByType("paint");if(a=jb(a,function(b){return"first-paint"===b.name}))return Mt(a.startTime)}a=S.timing;
return a.bd?Math.max(0,a.bd):0}
;function Yt(){var a=B("ytcsi.debug");a||(a=[],A("ytcsi.debug",a),A("ytcsi.reference",{}));return a}
function Zt(a){a=a||"";var b=B("ytcsi.reference");b||(Yt(),b=B("ytcsi.reference"));if(b[a])return b[a];var c=Yt(),d={timerName:a,info:{},tick:{},span:{},jspbInfo:[]};c.push(d);return b[a]=d}
;var T={},$t=(T.auto_search="LATENCY_ACTION_AUTO_SEARCH",T.ad_to_ad="LATENCY_ACTION_AD_TO_AD",T.ad_to_video="LATENCY_ACTION_AD_TO_VIDEO",T["analytics.explore"]="LATENCY_ACTION_CREATOR_ANALYTICS_EXPLORE",T.app_startup="LATENCY_ACTION_APP_STARTUP",T["artist.analytics"]="LATENCY_ACTION_CREATOR_ARTIST_ANALYTICS",T["artist.events"]="LATENCY_ACTION_CREATOR_ARTIST_CONCERTS",T["artist.presskit"]="LATENCY_ACTION_CREATOR_ARTIST_PROFILE",T["song.analytics"]="LATENCY_ACTION_CREATOR_SONG_ANALYTICS",T.browse="LATENCY_ACTION_BROWSE",
T.cast_splash="LATENCY_ACTION_CAST_SPLASH",T.channels="LATENCY_ACTION_CHANNELS",T.creator_channel_dashboard="LATENCY_ACTION_CREATOR_CHANNEL_DASHBOARD",T["channel.analytics"]="LATENCY_ACTION_CREATOR_CHANNEL_ANALYTICS",T["channel.comments"]="LATENCY_ACTION_CREATOR_CHANNEL_COMMENTS",T["channel.content"]="LATENCY_ACTION_CREATOR_POST_LIST",T["channel.content.promotions"]="LATENCY_ACTION_CREATOR_PROMOTION_LIST",T["channel.copyright"]="LATENCY_ACTION_CREATOR_CHANNEL_COPYRIGHT",T["channel.editing"]="LATENCY_ACTION_CREATOR_CHANNEL_EDITING",
T["channel.monetization"]="LATENCY_ACTION_CREATOR_CHANNEL_MONETIZATION",T["channel.music"]="LATENCY_ACTION_CREATOR_CHANNEL_MUSIC",T["channel.music_storefront"]="LATENCY_ACTION_CREATOR_CHANNEL_MUSIC_STOREFRONT",T["channel.playlists"]="LATENCY_ACTION_CREATOR_CHANNEL_PLAYLISTS",T["channel.translations"]="LATENCY_ACTION_CREATOR_CHANNEL_TRANSLATIONS",T["channel.videos"]="LATENCY_ACTION_CREATOR_CHANNEL_VIDEOS",T["channel.live_streaming"]="LATENCY_ACTION_CREATOR_LIVE_STREAMING",T.chips="LATENCY_ACTION_CHIPS",
T["dialog.copyright_strikes"]="LATENCY_ACTION_CREATOR_DIALOG_COPYRIGHT_STRIKES",T["dialog.video_copyright"]="LATENCY_ACTION_CREATOR_DIALOG_VIDEO_COPYRIGHT",T["dialog.uploads"]="LATENCY_ACTION_CREATOR_DIALOG_UPLOADS",T.direct_playback="LATENCY_ACTION_DIRECT_PLAYBACK",T.embed="LATENCY_ACTION_EMBED",T.entity_key_serialization_perf="LATENCY_ACTION_ENTITY_KEY_SERIALIZATION_PERF",T.entity_key_deserialization_perf="LATENCY_ACTION_ENTITY_KEY_DESERIALIZATION_PERF",T.explore="LATENCY_ACTION_EXPLORE",T.home=
"LATENCY_ACTION_HOME",T.library="LATENCY_ACTION_LIBRARY",T.live="LATENCY_ACTION_LIVE",T.live_pagination="LATENCY_ACTION_LIVE_PAGINATION",T.onboarding="LATENCY_ACTION_ONBOARDING",T["owner.issues"]="LATENCY_ACTION_CREATOR_CMS_ISSUES",T.parent_profile_settings="LATENCY_ACTION_KIDS_PARENT_PROFILE_SETTINGS",T.parent_tools_collection="LATENCY_ACTION_PARENT_TOOLS_COLLECTION",T.parent_tools_dashboard="LATENCY_ACTION_PARENT_TOOLS_DASHBOARD",T.player_att="LATENCY_ACTION_PLAYER_ATTESTATION",T["post.comments"]=
"LATENCY_ACTION_CREATOR_POST_COMMENTS",T["post.edit"]="LATENCY_ACTION_CREATOR_POST_EDIT",T.prebuffer="LATENCY_ACTION_PREBUFFER",T.prefetch="LATENCY_ACTION_PREFETCH",T.profile_settings="LATENCY_ACTION_KIDS_PROFILE_SETTINGS",T.profile_switcher="LATENCY_ACTION_LOGIN",T.reel_watch="LATENCY_ACTION_REEL_WATCH",T.results="LATENCY_ACTION_RESULTS",T["promotion.edit"]="LATENCY_ACTION_CREATOR_PROMOTION_EDIT",T.search_ui="LATENCY_ACTION_SEARCH_UI",T.search_suggest="LATENCY_ACTION_SUGGEST",T.search_zero_state=
"LATENCY_ACTION_SEARCH_ZERO_STATE",T.secret_code="LATENCY_ACTION_KIDS_SECRET_CODE",T.seek="LATENCY_ACTION_PLAYER_SEEK",T.settings="LATENCY_ACTION_SETTINGS",T.store="LATENCY_ACTION_STORE",T.tenx="LATENCY_ACTION_TENX",T.video_to_ad="LATENCY_ACTION_VIDEO_TO_AD",T.watch="LATENCY_ACTION_WATCH",T.watch_it_again="LATENCY_ACTION_KIDS_WATCH_IT_AGAIN",T["watch,watch7"]="LATENCY_ACTION_WATCH",T["watch,watch7_html5"]="LATENCY_ACTION_WATCH",T["watch,watch7ad"]="LATENCY_ACTION_WATCH",T["watch,watch7ad_html5"]=
"LATENCY_ACTION_WATCH",T.wn_comments="LATENCY_ACTION_LOAD_COMMENTS",T.ww_rqs="LATENCY_ACTION_WHO_IS_WATCHING",T["video.analytics"]="LATENCY_ACTION_CREATOR_VIDEO_ANALYTICS",T["video.comments"]="LATENCY_ACTION_CREATOR_VIDEO_COMMENTS",T["video.edit"]="LATENCY_ACTION_CREATOR_VIDEO_EDIT",T["video.editor"]="LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR",T["video.editor_async"]="LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR_ASYNC",T["video.live_settings"]="LATENCY_ACTION_CREATOR_VIDEO_LIVE_SETTINGS",T["video.live_streaming"]=
"LATENCY_ACTION_CREATOR_VIDEO_LIVE_STREAMING",T["video.monetization"]="LATENCY_ACTION_CREATOR_VIDEO_MONETIZATION",T["video.translations"]="LATENCY_ACTION_CREATOR_VIDEO_TRANSLATIONS",T.voice_assistant="LATENCY_ACTION_VOICE_ASSISTANT",T.cast_load_by_entity_to_watch="LATENCY_ACTION_CAST_LOAD_BY_ENTITY_TO_WATCH",T.networkless_performance="LATENCY_ACTION_NETWORKLESS_PERFORMANCE",T),V={},au=(V.ad_allowed="adTypesAllowed",V.yt_abt="adBreakType",V.ad_cpn="adClientPlaybackNonce",V.ad_docid="adVideoId",V.yt_ad_an=
"adNetworks",V.ad_at="adType",V.aida="appInstallDataAgeMs",V.browse_id="browseId",V.p="httpProtocol",V.t="transportProtocol",V.cs="commandSource",V.cpn="clientPlaybackNonce",V.ccs="creatorInfo.creatorCanaryState",V.ctop="creatorInfo.topEntityType",V.csn="clientScreenNonce",V.docid="videoId",V.GetHome_rid="requestIds",V.GetSearch_rid="requestIds",V.GetPlayer_rid="requestIds",V.GetWatchNext_rid="requestIds",V.GetBrowse_rid="requestIds",V.GetLibrary_rid="requestIds",V.is_continuation="isContinuation",
V.is_nav="isNavigation",V.b_p="kabukiInfo.browseParams",V.is_prefetch="kabukiInfo.isPrefetch",V.is_secondary_nav="kabukiInfo.isSecondaryNav",V.nav_type="kabukiInfo.navigationType",V.prev_browse_id="kabukiInfo.prevBrowseId",V.query_source="kabukiInfo.querySource",V.voz_type="kabukiInfo.vozType",V.yt_lt="loadType",V.mver="creatorInfo.measurementVersion",V.yt_ad="isMonetized",V.nr="webInfo.navigationReason",V.nrsu="navigationRequestedSameUrl",V.pnt="performanceNavigationTiming",V.prt="playbackRequiresTap",
V.plt="playerInfo.playbackType",V.pis="playerInfo.playerInitializedState",V.paused="playerInfo.isPausedOnLoad",V.yt_pt="playerType",V.fmt="playerInfo.itag",V.yt_pl="watchInfo.isPlaylist",V.yt_pre="playerInfo.preloadType",V.yt_ad_pr="prerollAllowed",V.pa="previousAction",V.yt_red="isRedSubscriber",V.rce="mwebInfo.responseContentEncoding",V.rc="resourceInfo.resourceCache",V.scrh="screenHeight",V.scrw="screenWidth",V.st="serverTimeMs",V.ssdm="shellStartupDurationMs",V.br_trs="tvInfo.bedrockTriggerState",
V.kebqat="kabukiInfo.earlyBrowseRequestInfo.abandonmentType",V.kebqa="kabukiInfo.earlyBrowseRequestInfo.adopted",V.label="tvInfo.label",V.is_mdx="tvInfo.isMdx",V.preloaded="tvInfo.isPreloaded",V.aac_type="tvInfo.authAccessCredentialType",V.upg_player_vis="playerInfo.visibilityState",V.query="unpluggedInfo.query",V.upg_chip_ids_string="unpluggedInfo.upgChipIdsString",V.yt_vst="videoStreamType",V.vph="viewportHeight",V.vpw="viewportWidth",V.yt_vis="isVisible",V.rcl="mwebInfo.responseContentLength",
V.GetSettings_rid="requestIds",V.GetTrending_rid="requestIds",V.GetMusicSearchSuggestions_rid="requestIds",V.REQUEST_ID="requestIds",V),bu="isContinuation isNavigation kabukiInfo.earlyBrowseRequestInfo.adopted kabukiInfo.isPrefetch kabukiInfo.isSecondaryNav isMonetized navigationRequestedSameUrl performanceNavigationTiming playerInfo.isPausedOnLoad prerollAllowed isRedSubscriber tvInfo.isMdx tvInfo.isPreloaded isVisible watchInfo.isPlaylist playbackRequiresTap".split(" "),cu={},du=(cu.ccs="CANARY_STATE_",
cu.mver="MEASUREMENT_VERSION_",cu.pis="PLAYER_INITIALIZED_STATE_",cu.yt_pt="LATENCY_PLAYER_",cu.pa="LATENCY_ACTION_",cu.ctop="TOP_ENTITY_TYPE_",cu.yt_vst="VIDEO_STREAM_TYPE_",cu),eu="all_vc ap aq c cbr cbrand cbrver cmodel cos cosver cplatform ctheme cver ei l_an l_mm plid srt yt_fss yt_li vpst vpni2 vpil2 icrc icrt pa GetAccountOverview_rid GetHistory_rid cmt d_vpct d_vpnfi d_vpni nsru pc pfa pfeh pftr pnc prerender psc rc start tcrt tcrc ssr vpr vps yt_abt yt_fn yt_fs yt_pft yt_pre yt_pt yt_pvis ytu_pvis yt_ref yt_sts tds".split(" ");
function fu(a){return $t[a]||"LATENCY_ACTION_UNKNOWN"}
function gu(a,b,c){c=St(c);if(c.gelInfos)c.gelInfos[a]=!0;else{var d={};c.gelInfos=(d[a]=!0,d)}if(a.match("_rid")){var e=a.split("_rid")[0];a="REQUEST_ID"}if(a in au){c=au[a];0<=eb(bu,c)&&(b=!!b);a in du&&"string"===typeof b&&(b=du[a]+b.toUpperCase());a=b;b=c.split(".");for(var f=d={},g=0;g<b.length-1;g++){var h=b[g];f[h]={};f=f[h]}f[b[b.length-1]]="requestIds"===c?[{id:a,endpoint:e}]:a;return xt({},d)}0<=eb(eu,a)||ms(new P("Unknown label logged with GEL CSI",a))}
;var W={LATENCY_ACTION_SHORTS_VIDEO_INGESTION_TRANSCODING:179,LATENCY_ACTION_KIDS_PROFILE_SWITCHER:90,LATENCY_ACTION_OFFLINE_THUMBNAIL_TRANSFER:100,LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR_ASYNC:46,LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR:37,LATENCY_ACTION_SPINNER_DISPLAYED:14,LATENCY_ACTION_PLAYABILITY_CHECK:10,LATENCY_ACTION_PROCESS:9,LATENCY_ACTION_APP_STARTUP:5,LATENCY_ACTION_COMMERCE_TRANSACTION:184,LATENCY_ACTION_LOG_PAYMENT_SERVER_ANALYTICS_RPC:173,LATENCY_ACTION_GET_PAYMENT_INSTRUMENTS_PARAMS_RPC:172,
LATENCY_ACTION_GET_FIX_INSTRUMENT_PARAMS_RPC:171,LATENCY_ACTION_RESUME_SUBSCRIPTION_RPC:170,LATENCY_ACTION_PAUSE_SUBSCRIPTION_RPC:169,LATENCY_ACTION_GET_OFFLINE_UPSELL_RPC:168,LATENCY_ACTION_GET_OFFERS_RPC:167,LATENCY_ACTION_GET_CANCELLATION_YT_FLOW_RPC:166,LATENCY_ACTION_GET_CANCELLATION_FLOW_RPC:165,LATENCY_ACTION_UPDATE_CROSS_DEVICE_OFFLINE_STATE_RPC:164,LATENCY_ACTION_GET_OFFER_DETAILS_RPC:163,LATENCY_ACTION_CANCEL_RECURRENCE_TRANSACTION_RPC:162,LATENCY_ACTION_GET_TIP_MODULE_RPC:161,LATENCY_ACTION_HANDLE_TRANSACTION_RPC:160,
LATENCY_ACTION_COMPLETE_TRANSACTION_RPC:159,LATENCY_ACTION_GET_CART_RPC:158,LATENCY_ACTION_THUMBNAIL_FETCH:156,LATENCY_ACTION_ABANDONED_DIRECT_PLAYBACK:154,LATENCY_ACTION_SHARE_VIDEO:153,LATENCY_ACTION_AD_TO_VIDEO_INT:152,LATENCY_ACTION_ABANDONED_BROWSE:151,LATENCY_ACTION_PLAYER_ROTATION:150,LATENCY_ACTION_GENERIC_WEB_VIEW:183,LATENCY_ACTION_SHOPPING_IN_APP:124,LATENCY_ACTION_PLAYER_ATTESTATION:121,LATENCY_ACTION_PLAYER_SEEK:119,LATENCY_ACTION_SUPER_STICKER_BUY_FLOW:114,LATENCY_ACTION_DOWNLOADS_DATA_ACCESS:180,
LATENCY_ACTION_BLOCKS_PERFORMANCE:148,LATENCY_ACTION_ASSISTANT_QUERY:138,LATENCY_ACTION_ASSISTANT_SETTINGS:137,LATENCY_ACTION_ENTITY_KEY_DESERIALIZATION_PERF:129,LATENCY_ACTION_ENTITY_KEY_SERIALIZATION_PERF:128,LATENCY_ACTION_PROOF_OF_ORIGIN_TOKEN_CREATE:127,LATENCY_ACTION_EMBEDS_SDK_INITIALIZATION:123,LATENCY_ACTION_NETWORKLESS_PERFORMANCE:122,LATENCY_ACTION_DOWNLOADS_EXPANSION:133,LATENCY_ACTION_ENTITY_TRANSFORM:131,LATENCY_ACTION_DOWNLOADS_COMPATIBILITY_LAYER:96,LATENCY_ACTION_EMBEDS_SET_VIDEO:95,
LATENCY_ACTION_SETTINGS:93,LATENCY_ACTION_ABANDONED_STARTUP:81,LATENCY_ACTION_MEDIA_BROWSER_ALARM_PLAY:80,LATENCY_ACTION_MEDIA_BROWSER_SEARCH:79,LATENCY_ACTION_MEDIA_BROWSER_LOAD_TREE:78,LATENCY_ACTION_WHO_IS_WATCHING:77,LATENCY_ACTION_CAST_LOAD_BY_ENTITY_TO_WATCH:76,LATENCY_ACTION_LITE_SWITCH_ACCOUNT:73,LATENCY_ACTION_ELEMENTS_PERFORMANCE:70,LATENCY_ACTION_LOCATION_SIGNAL_COLLECTION:69,LATENCY_ACTION_MODIFY_CHANNEL_NOTIFICATION:65,LATENCY_ACTION_OFFLINE_STORE_START:61,LATENCY_ACTION_REEL_EDITOR:58,
LATENCY_ACTION_CHANNEL_SUBSCRIBE:56,LATENCY_ACTION_CHANNEL_PREVIEW:55,LATENCY_ACTION_PREFETCH:52,LATENCY_ACTION_ABANDONED_WATCH:45,LATENCY_ACTION_LOAD_COMMENT_REPLIES:26,LATENCY_ACTION_LOAD_COMMENTS:25,LATENCY_ACTION_EDIT_COMMENT:24,LATENCY_ACTION_NEW_COMMENT:23,LATENCY_ACTION_OFFLINE_SHARING_RECEIVER_PAIRING:19,LATENCY_ACTION_EMBED:18,LATENCY_ACTION_MDX_LAUNCH:15,LATENCY_ACTION_RESOLVE_URL:13,LATENCY_ACTION_CAST_SPLASH:149,LATENCY_ACTION_MDX_CONNECT_TO_SESSION:190,LATENCY_ACTION_MDX_STREAM_TRANSFER:178,
LATENCY_ACTION_MDX_CAST:120,LATENCY_ACTION_MDX_COMMAND:12,LATENCY_ACTION_REEL_SELECT_SEGMENT:136,LATENCY_ACTION_ACCELERATED_EFFECTS:145,LATENCY_ACTION_EDIT_AUDIO_GEN:182,LATENCY_ACTION_UPLOAD_AUDIO_MIXER:147,LATENCY_ACTION_SHORTS_CLIENT_SIDE_RENDERING:157,LATENCY_ACTION_SHORTS_SEG_IMP_TRANSCODING:146,LATENCY_ACTION_SHORTS_AUDIO_PICKER_PLAYBACK:130,LATENCY_ACTION_SHORTS_WAVEFORM_DOWNLOAD:125,LATENCY_ACTION_SHORTS_VIDEO_INGESTION:155,LATENCY_ACTION_SHORTS_GALLERY:107,LATENCY_ACTION_SHORTS_TRIM:105,
LATENCY_ACTION_SHORTS_EDIT:104,LATENCY_ACTION_SHORTS_CAMERA:103,LATENCY_ACTION_PRODUCER_EXPORT_PROJECT:193,LATENCY_ACTION_PRODUCER_EDITOR:192,LATENCY_ACTION_PARENT_TOOLS_DASHBOARD:102,LATENCY_ACTION_PARENT_TOOLS_COLLECTION:101,LATENCY_ACTION_MUSIC_LOAD_RECOMMENDED_MEDIA_ITEMS:116,LATENCY_ACTION_MUSIC_LOAD_MEDIA_ITEMS:115,LATENCY_ACTION_MUSIC_ALBUM_DETAIL:72,LATENCY_ACTION_MUSIC_PLAYLIST_DETAIL:71,LATENCY_ACTION_STORE:175,LATENCY_ACTION_CHIPS:68,LATENCY_ACTION_SEARCH_ZERO_STATE:67,LATENCY_ACTION_LIVE_PAGINATION:117,
LATENCY_ACTION_LIVE:20,LATENCY_ACTION_PREBUFFER:40,LATENCY_ACTION_TENX:39,LATENCY_ACTION_KIDS_PROFILE_SETTINGS:94,LATENCY_ACTION_KIDS_WATCH_IT_AGAIN:92,LATENCY_ACTION_KIDS_SECRET_CODE:91,LATENCY_ACTION_KIDS_PARENT_PROFILE_SETTINGS:89,LATENCY_ACTION_KIDS_ONBOARDING:88,LATENCY_ACTION_KIDS_VOICE_SEARCH:82,LATENCY_ACTION_KIDS_CURATED_COLLECTION:62,LATENCY_ACTION_KIDS_LIBRARY:53,LATENCY_ACTION_CREATOR_PROMOTION_LIST:186,LATENCY_ACTION_CREATOR_PROMOTION_EDIT:185,LATENCY_ACTION_CREATOR_VIDEO_TRANSLATIONS:38,
LATENCY_ACTION_CREATOR_VIDEO_MONETIZATION:74,LATENCY_ACTION_CREATOR_VIDEO_LIVE_STREAMING:141,LATENCY_ACTION_CREATOR_VIDEO_LIVE_SETTINGS:142,LATENCY_ACTION_CREATOR_VIDEO_EDITOR_ASYNC:51,LATENCY_ACTION_CREATOR_VIDEO_EDITOR:50,LATENCY_ACTION_CREATOR_VIDEO_EDIT:36,LATENCY_ACTION_CREATOR_VIDEO_COMMENTS:34,LATENCY_ACTION_CREATOR_VIDEO_ANALYTICS:33,LATENCY_ACTION_CREATOR_SONG_ANALYTICS:176,LATENCY_ACTION_CREATOR_POST_LIST:112,LATENCY_ACTION_CREATOR_POST_EDIT:110,LATENCY_ACTION_CREATOR_POST_COMMENTS:111,
LATENCY_ACTION_CREATOR_LIVE_STREAMING:108,LATENCY_ACTION_CREATOR_DIALOG_VIDEO_COPYRIGHT:174,LATENCY_ACTION_CREATOR_DIALOG_UPLOADS:86,LATENCY_ACTION_CREATOR_DIALOG_COPYRIGHT_STRIKES:87,LATENCY_ACTION_CREATOR_CHANNEL_VIDEOS:32,LATENCY_ACTION_CREATOR_CHANNEL_TRANSLATIONS:48,LATENCY_ACTION_CREATOR_CHANNEL_PLAYLISTS:139,LATENCY_ACTION_CREATOR_CHANNEL_MUSIC_STOREFRONT:177,LATENCY_ACTION_CREATOR_CHANNEL_MUSIC:99,LATENCY_ACTION_CREATOR_CHANNEL_MONETIZATION:43,LATENCY_ACTION_CREATOR_CHANNEL_EDITING:113,LATENCY_ACTION_CREATOR_CHANNEL_DASHBOARD:49,
LATENCY_ACTION_CREATOR_CHANNEL_COPYRIGHT:44,LATENCY_ACTION_CREATOR_CMS_ISSUES:191,LATENCY_ACTION_CREATOR_CHANNEL_COMMENTS:66,LATENCY_ACTION_CREATOR_CHANNEL_ANALYTICS:31,LATENCY_ACTION_CREATOR_ARTIST_PROFILE:85,LATENCY_ACTION_CREATOR_ARTIST_CONCERTS:84,LATENCY_ACTION_CREATOR_ARTIST_ANALYTICS:83,LATENCY_ACTION_CREATOR_ANALYTICS_EXPLORE:140,LATENCY_ACTION_EXPERIMENTAL_WATCH_UI:181,LATENCY_ACTION_STORYBOARD_THUMBNAILS:118,LATENCY_ACTION_SEARCH_THUMBNAILS:59,LATENCY_ACTION_ON_DEVICE_MODEL_DOWNLOAD:54,
LATENCY_ACTION_VOICE_ASSISTANT:47,LATENCY_ACTION_SEARCH_UI:35,LATENCY_ACTION_SUGGEST:30,LATENCY_ACTION_AUTO_SEARCH:126,LATENCY_ACTION_DOWNLOADS:98,LATENCY_ACTION_EXPLORE:75,LATENCY_ACTION_VIDEO_LIST:63,LATENCY_ACTION_HOME_RESUME:60,LATENCY_ACTION_SUBSCRIPTIONS_LIST:57,LATENCY_ACTION_THUMBNAIL_LOAD:42,LATENCY_ACTION_FIRST_THUMBNAIL_LOAD:29,LATENCY_ACTION_SUBSCRIPTIONS_FEED:109,LATENCY_ACTION_SUBSCRIPTIONS:28,LATENCY_ACTION_TRENDING:27,LATENCY_ACTION_LIBRARY:21,LATENCY_ACTION_VIDEO_THUMBNAIL:8,LATENCY_ACTION_SHOW_MORE:7,
LATENCY_ACTION_VIDEO_PREVIEW:6,LATENCY_ACTION_AD_TO_AD:22,LATENCY_ACTION_VIDEO_TO_AD:17,LATENCY_ACTION_AD_TO_VIDEO:16,LATENCY_ACTION_DIRECT_PLAYBACK:132,LATENCY_ACTION_PREBUFFER_VIDEO:144,LATENCY_ACTION_PREFETCH_VIDEO:143,LATENCY_ACTION_STARTUP:106,LATENCY_ACTION_ONBOARDING:135,LATENCY_ACTION_LOGIN:97,LATENCY_ACTION_REEL_WATCH:41,LATENCY_ACTION_WATCH:3,LATENCY_ACTION_RESULTS:2,LATENCY_ACTION_CHANNELS:4,LATENCY_ACTION_HOME:1,LATENCY_ACTION_BROWSE:11,LATENCY_ACTION_USER_ACTION:189,LATENCY_ACTION_INFRASTRUCTURE:188,
LATENCY_ACTION_PAGE_NAVIGATION:187,LATENCY_ACTION_UNKNOWN:0};W[W.LATENCY_ACTION_SHORTS_VIDEO_INGESTION_TRANSCODING]="LATENCY_ACTION_SHORTS_VIDEO_INGESTION_TRANSCODING";W[W.LATENCY_ACTION_KIDS_PROFILE_SWITCHER]="LATENCY_ACTION_KIDS_PROFILE_SWITCHER";W[W.LATENCY_ACTION_OFFLINE_THUMBNAIL_TRANSFER]="LATENCY_ACTION_OFFLINE_THUMBNAIL_TRANSFER";W[W.LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR_ASYNC]="LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR_ASYNC";W[W.LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR]="LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR";
W[W.LATENCY_ACTION_SPINNER_DISPLAYED]="LATENCY_ACTION_SPINNER_DISPLAYED";W[W.LATENCY_ACTION_PLAYABILITY_CHECK]="LATENCY_ACTION_PLAYABILITY_CHECK";W[W.LATENCY_ACTION_PROCESS]="LATENCY_ACTION_PROCESS";W[W.LATENCY_ACTION_APP_STARTUP]="LATENCY_ACTION_APP_STARTUP";W[W.LATENCY_ACTION_COMMERCE_TRANSACTION]="LATENCY_ACTION_COMMERCE_TRANSACTION";W[W.LATENCY_ACTION_LOG_PAYMENT_SERVER_ANALYTICS_RPC]="LATENCY_ACTION_LOG_PAYMENT_SERVER_ANALYTICS_RPC";W[W.LATENCY_ACTION_GET_PAYMENT_INSTRUMENTS_PARAMS_RPC]="LATENCY_ACTION_GET_PAYMENT_INSTRUMENTS_PARAMS_RPC";
W[W.LATENCY_ACTION_GET_FIX_INSTRUMENT_PARAMS_RPC]="LATENCY_ACTION_GET_FIX_INSTRUMENT_PARAMS_RPC";W[W.LATENCY_ACTION_RESUME_SUBSCRIPTION_RPC]="LATENCY_ACTION_RESUME_SUBSCRIPTION_RPC";W[W.LATENCY_ACTION_PAUSE_SUBSCRIPTION_RPC]="LATENCY_ACTION_PAUSE_SUBSCRIPTION_RPC";W[W.LATENCY_ACTION_GET_OFFLINE_UPSELL_RPC]="LATENCY_ACTION_GET_OFFLINE_UPSELL_RPC";W[W.LATENCY_ACTION_GET_OFFERS_RPC]="LATENCY_ACTION_GET_OFFERS_RPC";W[W.LATENCY_ACTION_GET_CANCELLATION_YT_FLOW_RPC]="LATENCY_ACTION_GET_CANCELLATION_YT_FLOW_RPC";
W[W.LATENCY_ACTION_GET_CANCELLATION_FLOW_RPC]="LATENCY_ACTION_GET_CANCELLATION_FLOW_RPC";W[W.LATENCY_ACTION_UPDATE_CROSS_DEVICE_OFFLINE_STATE_RPC]="LATENCY_ACTION_UPDATE_CROSS_DEVICE_OFFLINE_STATE_RPC";W[W.LATENCY_ACTION_GET_OFFER_DETAILS_RPC]="LATENCY_ACTION_GET_OFFER_DETAILS_RPC";W[W.LATENCY_ACTION_CANCEL_RECURRENCE_TRANSACTION_RPC]="LATENCY_ACTION_CANCEL_RECURRENCE_TRANSACTION_RPC";W[W.LATENCY_ACTION_GET_TIP_MODULE_RPC]="LATENCY_ACTION_GET_TIP_MODULE_RPC";
W[W.LATENCY_ACTION_HANDLE_TRANSACTION_RPC]="LATENCY_ACTION_HANDLE_TRANSACTION_RPC";W[W.LATENCY_ACTION_COMPLETE_TRANSACTION_RPC]="LATENCY_ACTION_COMPLETE_TRANSACTION_RPC";W[W.LATENCY_ACTION_GET_CART_RPC]="LATENCY_ACTION_GET_CART_RPC";W[W.LATENCY_ACTION_THUMBNAIL_FETCH]="LATENCY_ACTION_THUMBNAIL_FETCH";W[W.LATENCY_ACTION_ABANDONED_DIRECT_PLAYBACK]="LATENCY_ACTION_ABANDONED_DIRECT_PLAYBACK";W[W.LATENCY_ACTION_SHARE_VIDEO]="LATENCY_ACTION_SHARE_VIDEO";W[W.LATENCY_ACTION_AD_TO_VIDEO_INT]="LATENCY_ACTION_AD_TO_VIDEO_INT";
W[W.LATENCY_ACTION_ABANDONED_BROWSE]="LATENCY_ACTION_ABANDONED_BROWSE";W[W.LATENCY_ACTION_PLAYER_ROTATION]="LATENCY_ACTION_PLAYER_ROTATION";W[W.LATENCY_ACTION_GENERIC_WEB_VIEW]="LATENCY_ACTION_GENERIC_WEB_VIEW";W[W.LATENCY_ACTION_SHOPPING_IN_APP]="LATENCY_ACTION_SHOPPING_IN_APP";W[W.LATENCY_ACTION_PLAYER_ATTESTATION]="LATENCY_ACTION_PLAYER_ATTESTATION";W[W.LATENCY_ACTION_PLAYER_SEEK]="LATENCY_ACTION_PLAYER_SEEK";W[W.LATENCY_ACTION_SUPER_STICKER_BUY_FLOW]="LATENCY_ACTION_SUPER_STICKER_BUY_FLOW";
W[W.LATENCY_ACTION_DOWNLOADS_DATA_ACCESS]="LATENCY_ACTION_DOWNLOADS_DATA_ACCESS";W[W.LATENCY_ACTION_BLOCKS_PERFORMANCE]="LATENCY_ACTION_BLOCKS_PERFORMANCE";W[W.LATENCY_ACTION_ASSISTANT_QUERY]="LATENCY_ACTION_ASSISTANT_QUERY";W[W.LATENCY_ACTION_ASSISTANT_SETTINGS]="LATENCY_ACTION_ASSISTANT_SETTINGS";W[W.LATENCY_ACTION_ENTITY_KEY_DESERIALIZATION_PERF]="LATENCY_ACTION_ENTITY_KEY_DESERIALIZATION_PERF";W[W.LATENCY_ACTION_ENTITY_KEY_SERIALIZATION_PERF]="LATENCY_ACTION_ENTITY_KEY_SERIALIZATION_PERF";
W[W.LATENCY_ACTION_PROOF_OF_ORIGIN_TOKEN_CREATE]="LATENCY_ACTION_PROOF_OF_ORIGIN_TOKEN_CREATE";W[W.LATENCY_ACTION_EMBEDS_SDK_INITIALIZATION]="LATENCY_ACTION_EMBEDS_SDK_INITIALIZATION";W[W.LATENCY_ACTION_NETWORKLESS_PERFORMANCE]="LATENCY_ACTION_NETWORKLESS_PERFORMANCE";W[W.LATENCY_ACTION_DOWNLOADS_EXPANSION]="LATENCY_ACTION_DOWNLOADS_EXPANSION";W[W.LATENCY_ACTION_ENTITY_TRANSFORM]="LATENCY_ACTION_ENTITY_TRANSFORM";W[W.LATENCY_ACTION_DOWNLOADS_COMPATIBILITY_LAYER]="LATENCY_ACTION_DOWNLOADS_COMPATIBILITY_LAYER";
W[W.LATENCY_ACTION_EMBEDS_SET_VIDEO]="LATENCY_ACTION_EMBEDS_SET_VIDEO";W[W.LATENCY_ACTION_SETTINGS]="LATENCY_ACTION_SETTINGS";W[W.LATENCY_ACTION_ABANDONED_STARTUP]="LATENCY_ACTION_ABANDONED_STARTUP";W[W.LATENCY_ACTION_MEDIA_BROWSER_ALARM_PLAY]="LATENCY_ACTION_MEDIA_BROWSER_ALARM_PLAY";W[W.LATENCY_ACTION_MEDIA_BROWSER_SEARCH]="LATENCY_ACTION_MEDIA_BROWSER_SEARCH";W[W.LATENCY_ACTION_MEDIA_BROWSER_LOAD_TREE]="LATENCY_ACTION_MEDIA_BROWSER_LOAD_TREE";W[W.LATENCY_ACTION_WHO_IS_WATCHING]="LATENCY_ACTION_WHO_IS_WATCHING";
W[W.LATENCY_ACTION_CAST_LOAD_BY_ENTITY_TO_WATCH]="LATENCY_ACTION_CAST_LOAD_BY_ENTITY_TO_WATCH";W[W.LATENCY_ACTION_LITE_SWITCH_ACCOUNT]="LATENCY_ACTION_LITE_SWITCH_ACCOUNT";W[W.LATENCY_ACTION_ELEMENTS_PERFORMANCE]="LATENCY_ACTION_ELEMENTS_PERFORMANCE";W[W.LATENCY_ACTION_LOCATION_SIGNAL_COLLECTION]="LATENCY_ACTION_LOCATION_SIGNAL_COLLECTION";W[W.LATENCY_ACTION_MODIFY_CHANNEL_NOTIFICATION]="LATENCY_ACTION_MODIFY_CHANNEL_NOTIFICATION";W[W.LATENCY_ACTION_OFFLINE_STORE_START]="LATENCY_ACTION_OFFLINE_STORE_START";
W[W.LATENCY_ACTION_REEL_EDITOR]="LATENCY_ACTION_REEL_EDITOR";W[W.LATENCY_ACTION_CHANNEL_SUBSCRIBE]="LATENCY_ACTION_CHANNEL_SUBSCRIBE";W[W.LATENCY_ACTION_CHANNEL_PREVIEW]="LATENCY_ACTION_CHANNEL_PREVIEW";W[W.LATENCY_ACTION_PREFETCH]="LATENCY_ACTION_PREFETCH";W[W.LATENCY_ACTION_ABANDONED_WATCH]="LATENCY_ACTION_ABANDONED_WATCH";W[W.LATENCY_ACTION_LOAD_COMMENT_REPLIES]="LATENCY_ACTION_LOAD_COMMENT_REPLIES";W[W.LATENCY_ACTION_LOAD_COMMENTS]="LATENCY_ACTION_LOAD_COMMENTS";
W[W.LATENCY_ACTION_EDIT_COMMENT]="LATENCY_ACTION_EDIT_COMMENT";W[W.LATENCY_ACTION_NEW_COMMENT]="LATENCY_ACTION_NEW_COMMENT";W[W.LATENCY_ACTION_OFFLINE_SHARING_RECEIVER_PAIRING]="LATENCY_ACTION_OFFLINE_SHARING_RECEIVER_PAIRING";W[W.LATENCY_ACTION_EMBED]="LATENCY_ACTION_EMBED";W[W.LATENCY_ACTION_MDX_LAUNCH]="LATENCY_ACTION_MDX_LAUNCH";W[W.LATENCY_ACTION_RESOLVE_URL]="LATENCY_ACTION_RESOLVE_URL";W[W.LATENCY_ACTION_CAST_SPLASH]="LATENCY_ACTION_CAST_SPLASH";W[W.LATENCY_ACTION_MDX_CONNECT_TO_SESSION]="LATENCY_ACTION_MDX_CONNECT_TO_SESSION";
W[W.LATENCY_ACTION_MDX_STREAM_TRANSFER]="LATENCY_ACTION_MDX_STREAM_TRANSFER";W[W.LATENCY_ACTION_MDX_CAST]="LATENCY_ACTION_MDX_CAST";W[W.LATENCY_ACTION_MDX_COMMAND]="LATENCY_ACTION_MDX_COMMAND";W[W.LATENCY_ACTION_REEL_SELECT_SEGMENT]="LATENCY_ACTION_REEL_SELECT_SEGMENT";W[W.LATENCY_ACTION_ACCELERATED_EFFECTS]="LATENCY_ACTION_ACCELERATED_EFFECTS";W[W.LATENCY_ACTION_EDIT_AUDIO_GEN]="LATENCY_ACTION_EDIT_AUDIO_GEN";W[W.LATENCY_ACTION_UPLOAD_AUDIO_MIXER]="LATENCY_ACTION_UPLOAD_AUDIO_MIXER";
W[W.LATENCY_ACTION_SHORTS_CLIENT_SIDE_RENDERING]="LATENCY_ACTION_SHORTS_CLIENT_SIDE_RENDERING";W[W.LATENCY_ACTION_SHORTS_SEG_IMP_TRANSCODING]="LATENCY_ACTION_SHORTS_SEG_IMP_TRANSCODING";W[W.LATENCY_ACTION_SHORTS_AUDIO_PICKER_PLAYBACK]="LATENCY_ACTION_SHORTS_AUDIO_PICKER_PLAYBACK";W[W.LATENCY_ACTION_SHORTS_WAVEFORM_DOWNLOAD]="LATENCY_ACTION_SHORTS_WAVEFORM_DOWNLOAD";W[W.LATENCY_ACTION_SHORTS_VIDEO_INGESTION]="LATENCY_ACTION_SHORTS_VIDEO_INGESTION";W[W.LATENCY_ACTION_SHORTS_GALLERY]="LATENCY_ACTION_SHORTS_GALLERY";
W[W.LATENCY_ACTION_SHORTS_TRIM]="LATENCY_ACTION_SHORTS_TRIM";W[W.LATENCY_ACTION_SHORTS_EDIT]="LATENCY_ACTION_SHORTS_EDIT";W[W.LATENCY_ACTION_SHORTS_CAMERA]="LATENCY_ACTION_SHORTS_CAMERA";W[W.LATENCY_ACTION_PRODUCER_EXPORT_PROJECT]="LATENCY_ACTION_PRODUCER_EXPORT_PROJECT";W[W.LATENCY_ACTION_PRODUCER_EDITOR]="LATENCY_ACTION_PRODUCER_EDITOR";W[W.LATENCY_ACTION_PARENT_TOOLS_DASHBOARD]="LATENCY_ACTION_PARENT_TOOLS_DASHBOARD";W[W.LATENCY_ACTION_PARENT_TOOLS_COLLECTION]="LATENCY_ACTION_PARENT_TOOLS_COLLECTION";
W[W.LATENCY_ACTION_MUSIC_LOAD_RECOMMENDED_MEDIA_ITEMS]="LATENCY_ACTION_MUSIC_LOAD_RECOMMENDED_MEDIA_ITEMS";W[W.LATENCY_ACTION_MUSIC_LOAD_MEDIA_ITEMS]="LATENCY_ACTION_MUSIC_LOAD_MEDIA_ITEMS";W[W.LATENCY_ACTION_MUSIC_ALBUM_DETAIL]="LATENCY_ACTION_MUSIC_ALBUM_DETAIL";W[W.LATENCY_ACTION_MUSIC_PLAYLIST_DETAIL]="LATENCY_ACTION_MUSIC_PLAYLIST_DETAIL";W[W.LATENCY_ACTION_STORE]="LATENCY_ACTION_STORE";W[W.LATENCY_ACTION_CHIPS]="LATENCY_ACTION_CHIPS";W[W.LATENCY_ACTION_SEARCH_ZERO_STATE]="LATENCY_ACTION_SEARCH_ZERO_STATE";
W[W.LATENCY_ACTION_LIVE_PAGINATION]="LATENCY_ACTION_LIVE_PAGINATION";W[W.LATENCY_ACTION_LIVE]="LATENCY_ACTION_LIVE";W[W.LATENCY_ACTION_PREBUFFER]="LATENCY_ACTION_PREBUFFER";W[W.LATENCY_ACTION_TENX]="LATENCY_ACTION_TENX";W[W.LATENCY_ACTION_KIDS_PROFILE_SETTINGS]="LATENCY_ACTION_KIDS_PROFILE_SETTINGS";W[W.LATENCY_ACTION_KIDS_WATCH_IT_AGAIN]="LATENCY_ACTION_KIDS_WATCH_IT_AGAIN";W[W.LATENCY_ACTION_KIDS_SECRET_CODE]="LATENCY_ACTION_KIDS_SECRET_CODE";W[W.LATENCY_ACTION_KIDS_PARENT_PROFILE_SETTINGS]="LATENCY_ACTION_KIDS_PARENT_PROFILE_SETTINGS";
W[W.LATENCY_ACTION_KIDS_ONBOARDING]="LATENCY_ACTION_KIDS_ONBOARDING";W[W.LATENCY_ACTION_KIDS_VOICE_SEARCH]="LATENCY_ACTION_KIDS_VOICE_SEARCH";W[W.LATENCY_ACTION_KIDS_CURATED_COLLECTION]="LATENCY_ACTION_KIDS_CURATED_COLLECTION";W[W.LATENCY_ACTION_KIDS_LIBRARY]="LATENCY_ACTION_KIDS_LIBRARY";W[W.LATENCY_ACTION_CREATOR_PROMOTION_LIST]="LATENCY_ACTION_CREATOR_PROMOTION_LIST";W[W.LATENCY_ACTION_CREATOR_PROMOTION_EDIT]="LATENCY_ACTION_CREATOR_PROMOTION_EDIT";
W[W.LATENCY_ACTION_CREATOR_VIDEO_TRANSLATIONS]="LATENCY_ACTION_CREATOR_VIDEO_TRANSLATIONS";W[W.LATENCY_ACTION_CREATOR_VIDEO_MONETIZATION]="LATENCY_ACTION_CREATOR_VIDEO_MONETIZATION";W[W.LATENCY_ACTION_CREATOR_VIDEO_LIVE_STREAMING]="LATENCY_ACTION_CREATOR_VIDEO_LIVE_STREAMING";W[W.LATENCY_ACTION_CREATOR_VIDEO_LIVE_SETTINGS]="LATENCY_ACTION_CREATOR_VIDEO_LIVE_SETTINGS";W[W.LATENCY_ACTION_CREATOR_VIDEO_EDITOR_ASYNC]="LATENCY_ACTION_CREATOR_VIDEO_EDITOR_ASYNC";
W[W.LATENCY_ACTION_CREATOR_VIDEO_EDITOR]="LATENCY_ACTION_CREATOR_VIDEO_EDITOR";W[W.LATENCY_ACTION_CREATOR_VIDEO_EDIT]="LATENCY_ACTION_CREATOR_VIDEO_EDIT";W[W.LATENCY_ACTION_CREATOR_VIDEO_COMMENTS]="LATENCY_ACTION_CREATOR_VIDEO_COMMENTS";W[W.LATENCY_ACTION_CREATOR_VIDEO_ANALYTICS]="LATENCY_ACTION_CREATOR_VIDEO_ANALYTICS";W[W.LATENCY_ACTION_CREATOR_SONG_ANALYTICS]="LATENCY_ACTION_CREATOR_SONG_ANALYTICS";W[W.LATENCY_ACTION_CREATOR_POST_LIST]="LATENCY_ACTION_CREATOR_POST_LIST";
W[W.LATENCY_ACTION_CREATOR_POST_EDIT]="LATENCY_ACTION_CREATOR_POST_EDIT";W[W.LATENCY_ACTION_CREATOR_POST_COMMENTS]="LATENCY_ACTION_CREATOR_POST_COMMENTS";W[W.LATENCY_ACTION_CREATOR_LIVE_STREAMING]="LATENCY_ACTION_CREATOR_LIVE_STREAMING";W[W.LATENCY_ACTION_CREATOR_DIALOG_VIDEO_COPYRIGHT]="LATENCY_ACTION_CREATOR_DIALOG_VIDEO_COPYRIGHT";W[W.LATENCY_ACTION_CREATOR_DIALOG_UPLOADS]="LATENCY_ACTION_CREATOR_DIALOG_UPLOADS";W[W.LATENCY_ACTION_CREATOR_DIALOG_COPYRIGHT_STRIKES]="LATENCY_ACTION_CREATOR_DIALOG_COPYRIGHT_STRIKES";
W[W.LATENCY_ACTION_CREATOR_CHANNEL_VIDEOS]="LATENCY_ACTION_CREATOR_CHANNEL_VIDEOS";W[W.LATENCY_ACTION_CREATOR_CHANNEL_TRANSLATIONS]="LATENCY_ACTION_CREATOR_CHANNEL_TRANSLATIONS";W[W.LATENCY_ACTION_CREATOR_CHANNEL_PLAYLISTS]="LATENCY_ACTION_CREATOR_CHANNEL_PLAYLISTS";W[W.LATENCY_ACTION_CREATOR_CHANNEL_MUSIC_STOREFRONT]="LATENCY_ACTION_CREATOR_CHANNEL_MUSIC_STOREFRONT";W[W.LATENCY_ACTION_CREATOR_CHANNEL_MUSIC]="LATENCY_ACTION_CREATOR_CHANNEL_MUSIC";W[W.LATENCY_ACTION_CREATOR_CHANNEL_MONETIZATION]="LATENCY_ACTION_CREATOR_CHANNEL_MONETIZATION";
W[W.LATENCY_ACTION_CREATOR_CHANNEL_EDITING]="LATENCY_ACTION_CREATOR_CHANNEL_EDITING";W[W.LATENCY_ACTION_CREATOR_CHANNEL_DASHBOARD]="LATENCY_ACTION_CREATOR_CHANNEL_DASHBOARD";W[W.LATENCY_ACTION_CREATOR_CHANNEL_COPYRIGHT]="LATENCY_ACTION_CREATOR_CHANNEL_COPYRIGHT";W[W.LATENCY_ACTION_CREATOR_CMS_ISSUES]="LATENCY_ACTION_CREATOR_CMS_ISSUES";W[W.LATENCY_ACTION_CREATOR_CHANNEL_COMMENTS]="LATENCY_ACTION_CREATOR_CHANNEL_COMMENTS";W[W.LATENCY_ACTION_CREATOR_CHANNEL_ANALYTICS]="LATENCY_ACTION_CREATOR_CHANNEL_ANALYTICS";
W[W.LATENCY_ACTION_CREATOR_ARTIST_PROFILE]="LATENCY_ACTION_CREATOR_ARTIST_PROFILE";W[W.LATENCY_ACTION_CREATOR_ARTIST_CONCERTS]="LATENCY_ACTION_CREATOR_ARTIST_CONCERTS";W[W.LATENCY_ACTION_CREATOR_ARTIST_ANALYTICS]="LATENCY_ACTION_CREATOR_ARTIST_ANALYTICS";W[W.LATENCY_ACTION_CREATOR_ANALYTICS_EXPLORE]="LATENCY_ACTION_CREATOR_ANALYTICS_EXPLORE";W[W.LATENCY_ACTION_EXPERIMENTAL_WATCH_UI]="LATENCY_ACTION_EXPERIMENTAL_WATCH_UI";W[W.LATENCY_ACTION_STORYBOARD_THUMBNAILS]="LATENCY_ACTION_STORYBOARD_THUMBNAILS";
W[W.LATENCY_ACTION_SEARCH_THUMBNAILS]="LATENCY_ACTION_SEARCH_THUMBNAILS";W[W.LATENCY_ACTION_ON_DEVICE_MODEL_DOWNLOAD]="LATENCY_ACTION_ON_DEVICE_MODEL_DOWNLOAD";W[W.LATENCY_ACTION_VOICE_ASSISTANT]="LATENCY_ACTION_VOICE_ASSISTANT";W[W.LATENCY_ACTION_SEARCH_UI]="LATENCY_ACTION_SEARCH_UI";W[W.LATENCY_ACTION_SUGGEST]="LATENCY_ACTION_SUGGEST";W[W.LATENCY_ACTION_AUTO_SEARCH]="LATENCY_ACTION_AUTO_SEARCH";W[W.LATENCY_ACTION_DOWNLOADS]="LATENCY_ACTION_DOWNLOADS";W[W.LATENCY_ACTION_EXPLORE]="LATENCY_ACTION_EXPLORE";
W[W.LATENCY_ACTION_VIDEO_LIST]="LATENCY_ACTION_VIDEO_LIST";W[W.LATENCY_ACTION_HOME_RESUME]="LATENCY_ACTION_HOME_RESUME";W[W.LATENCY_ACTION_SUBSCRIPTIONS_LIST]="LATENCY_ACTION_SUBSCRIPTIONS_LIST";W[W.LATENCY_ACTION_THUMBNAIL_LOAD]="LATENCY_ACTION_THUMBNAIL_LOAD";W[W.LATENCY_ACTION_FIRST_THUMBNAIL_LOAD]="LATENCY_ACTION_FIRST_THUMBNAIL_LOAD";W[W.LATENCY_ACTION_SUBSCRIPTIONS_FEED]="LATENCY_ACTION_SUBSCRIPTIONS_FEED";W[W.LATENCY_ACTION_SUBSCRIPTIONS]="LATENCY_ACTION_SUBSCRIPTIONS";
W[W.LATENCY_ACTION_TRENDING]="LATENCY_ACTION_TRENDING";W[W.LATENCY_ACTION_LIBRARY]="LATENCY_ACTION_LIBRARY";W[W.LATENCY_ACTION_VIDEO_THUMBNAIL]="LATENCY_ACTION_VIDEO_THUMBNAIL";W[W.LATENCY_ACTION_SHOW_MORE]="LATENCY_ACTION_SHOW_MORE";W[W.LATENCY_ACTION_VIDEO_PREVIEW]="LATENCY_ACTION_VIDEO_PREVIEW";W[W.LATENCY_ACTION_AD_TO_AD]="LATENCY_ACTION_AD_TO_AD";W[W.LATENCY_ACTION_VIDEO_TO_AD]="LATENCY_ACTION_VIDEO_TO_AD";W[W.LATENCY_ACTION_AD_TO_VIDEO]="LATENCY_ACTION_AD_TO_VIDEO";
W[W.LATENCY_ACTION_DIRECT_PLAYBACK]="LATENCY_ACTION_DIRECT_PLAYBACK";W[W.LATENCY_ACTION_PREBUFFER_VIDEO]="LATENCY_ACTION_PREBUFFER_VIDEO";W[W.LATENCY_ACTION_PREFETCH_VIDEO]="LATENCY_ACTION_PREFETCH_VIDEO";W[W.LATENCY_ACTION_STARTUP]="LATENCY_ACTION_STARTUP";W[W.LATENCY_ACTION_ONBOARDING]="LATENCY_ACTION_ONBOARDING";W[W.LATENCY_ACTION_LOGIN]="LATENCY_ACTION_LOGIN";W[W.LATENCY_ACTION_REEL_WATCH]="LATENCY_ACTION_REEL_WATCH";W[W.LATENCY_ACTION_WATCH]="LATENCY_ACTION_WATCH";
W[W.LATENCY_ACTION_RESULTS]="LATENCY_ACTION_RESULTS";W[W.LATENCY_ACTION_CHANNELS]="LATENCY_ACTION_CHANNELS";W[W.LATENCY_ACTION_HOME]="LATENCY_ACTION_HOME";W[W.LATENCY_ACTION_BROWSE]="LATENCY_ACTION_BROWSE";W[W.LATENCY_ACTION_USER_ACTION]="LATENCY_ACTION_USER_ACTION";W[W.LATENCY_ACTION_INFRASTRUCTURE]="LATENCY_ACTION_INFRASTRUCTURE";W[W.LATENCY_ACTION_PAGE_NAVIGATION]="LATENCY_ACTION_PAGE_NAVIGATION";W[W.LATENCY_ACTION_UNKNOWN]="LATENCY_ACTION_UNKNOWN";
var hu={LATENCY_NETWORK_MOBILE:2,LATENCY_NETWORK_WIFI:1,LATENCY_NETWORK_UNKNOWN:0};hu[hu.LATENCY_NETWORK_MOBILE]="LATENCY_NETWORK_MOBILE";hu[hu.LATENCY_NETWORK_WIFI]="LATENCY_NETWORK_WIFI";hu[hu.LATENCY_NETWORK_UNKNOWN]="LATENCY_NETWORK_UNKNOWN";var X={CONN_INVALID:31,CONN_CELLULAR_5G_NSA:12,CONN_CELLULAR_5G_SA:11,CONN_WIFI_METERED:10,CONN_CELLULAR_5G:9,CONN_DISCO:8,CONN_CELLULAR_UNKNOWN:7,CONN_CELLULAR_4G:6,CONN_CELLULAR_3G:5,CONN_CELLULAR_2G:4,CONN_WIFI:3,CONN_NONE:2,CONN_UNKNOWN:1,CONN_DEFAULT:0};
X[X.CONN_INVALID]="CONN_INVALID";X[X.CONN_CELLULAR_5G_NSA]="CONN_CELLULAR_5G_NSA";X[X.CONN_CELLULAR_5G_SA]="CONN_CELLULAR_5G_SA";X[X.CONN_WIFI_METERED]="CONN_WIFI_METERED";X[X.CONN_CELLULAR_5G]="CONN_CELLULAR_5G";X[X.CONN_DISCO]="CONN_DISCO";X[X.CONN_CELLULAR_UNKNOWN]="CONN_CELLULAR_UNKNOWN";X[X.CONN_CELLULAR_4G]="CONN_CELLULAR_4G";X[X.CONN_CELLULAR_3G]="CONN_CELLULAR_3G";X[X.CONN_CELLULAR_2G]="CONN_CELLULAR_2G";X[X.CONN_WIFI]="CONN_WIFI";X[X.CONN_NONE]="CONN_NONE";X[X.CONN_UNKNOWN]="CONN_UNKNOWN";
X[X.CONN_DEFAULT]="CONN_DEFAULT";
var Y={DETAILED_NETWORK_TYPE_NR_NSA:126,DETAILED_NETWORK_TYPE_NR_SA:125,DETAILED_NETWORK_TYPE_INTERNAL_WIFI_IMPAIRED:124,DETAILED_NETWORK_TYPE_APP_WIFI_HOTSPOT:123,DETAILED_NETWORK_TYPE_DISCONNECTED:122,DETAILED_NETWORK_TYPE_NON_MOBILE_UNKNOWN:121,DETAILED_NETWORK_TYPE_MOBILE_UNKNOWN:120,DETAILED_NETWORK_TYPE_WIMAX:119,DETAILED_NETWORK_TYPE_ETHERNET:118,DETAILED_NETWORK_TYPE_BLUETOOTH:117,DETAILED_NETWORK_TYPE_WIFI:116,DETAILED_NETWORK_TYPE_LTE:115,DETAILED_NETWORK_TYPE_HSPAP:114,DETAILED_NETWORK_TYPE_EHRPD:113,
DETAILED_NETWORK_TYPE_EVDO_B:112,DETAILED_NETWORK_TYPE_UMTS:111,DETAILED_NETWORK_TYPE_IDEN:110,DETAILED_NETWORK_TYPE_HSUPA:109,DETAILED_NETWORK_TYPE_HSPA:108,DETAILED_NETWORK_TYPE_HSDPA:107,DETAILED_NETWORK_TYPE_EVDO_A:106,DETAILED_NETWORK_TYPE_EVDO_0:105,DETAILED_NETWORK_TYPE_CDMA:104,DETAILED_NETWORK_TYPE_1_X_RTT:103,DETAILED_NETWORK_TYPE_GPRS:102,DETAILED_NETWORK_TYPE_EDGE:101,DETAILED_NETWORK_TYPE_UNKNOWN:0};Y[Y.DETAILED_NETWORK_TYPE_NR_NSA]="DETAILED_NETWORK_TYPE_NR_NSA";
Y[Y.DETAILED_NETWORK_TYPE_NR_SA]="DETAILED_NETWORK_TYPE_NR_SA";Y[Y.DETAILED_NETWORK_TYPE_INTERNAL_WIFI_IMPAIRED]="DETAILED_NETWORK_TYPE_INTERNAL_WIFI_IMPAIRED";Y[Y.DETAILED_NETWORK_TYPE_APP_WIFI_HOTSPOT]="DETAILED_NETWORK_TYPE_APP_WIFI_HOTSPOT";Y[Y.DETAILED_NETWORK_TYPE_DISCONNECTED]="DETAILED_NETWORK_TYPE_DISCONNECTED";Y[Y.DETAILED_NETWORK_TYPE_NON_MOBILE_UNKNOWN]="DETAILED_NETWORK_TYPE_NON_MOBILE_UNKNOWN";Y[Y.DETAILED_NETWORK_TYPE_MOBILE_UNKNOWN]="DETAILED_NETWORK_TYPE_MOBILE_UNKNOWN";
Y[Y.DETAILED_NETWORK_TYPE_WIMAX]="DETAILED_NETWORK_TYPE_WIMAX";Y[Y.DETAILED_NETWORK_TYPE_ETHERNET]="DETAILED_NETWORK_TYPE_ETHERNET";Y[Y.DETAILED_NETWORK_TYPE_BLUETOOTH]="DETAILED_NETWORK_TYPE_BLUETOOTH";Y[Y.DETAILED_NETWORK_TYPE_WIFI]="DETAILED_NETWORK_TYPE_WIFI";Y[Y.DETAILED_NETWORK_TYPE_LTE]="DETAILED_NETWORK_TYPE_LTE";Y[Y.DETAILED_NETWORK_TYPE_HSPAP]="DETAILED_NETWORK_TYPE_HSPAP";Y[Y.DETAILED_NETWORK_TYPE_EHRPD]="DETAILED_NETWORK_TYPE_EHRPD";Y[Y.DETAILED_NETWORK_TYPE_EVDO_B]="DETAILED_NETWORK_TYPE_EVDO_B";
Y[Y.DETAILED_NETWORK_TYPE_UMTS]="DETAILED_NETWORK_TYPE_UMTS";Y[Y.DETAILED_NETWORK_TYPE_IDEN]="DETAILED_NETWORK_TYPE_IDEN";Y[Y.DETAILED_NETWORK_TYPE_HSUPA]="DETAILED_NETWORK_TYPE_HSUPA";Y[Y.DETAILED_NETWORK_TYPE_HSPA]="DETAILED_NETWORK_TYPE_HSPA";Y[Y.DETAILED_NETWORK_TYPE_HSDPA]="DETAILED_NETWORK_TYPE_HSDPA";Y[Y.DETAILED_NETWORK_TYPE_EVDO_A]="DETAILED_NETWORK_TYPE_EVDO_A";Y[Y.DETAILED_NETWORK_TYPE_EVDO_0]="DETAILED_NETWORK_TYPE_EVDO_0";Y[Y.DETAILED_NETWORK_TYPE_CDMA]="DETAILED_NETWORK_TYPE_CDMA";
Y[Y.DETAILED_NETWORK_TYPE_1_X_RTT]="DETAILED_NETWORK_TYPE_1_X_RTT";Y[Y.DETAILED_NETWORK_TYPE_GPRS]="DETAILED_NETWORK_TYPE_GPRS";Y[Y.DETAILED_NETWORK_TYPE_EDGE]="DETAILED_NETWORK_TYPE_EDGE";Y[Y.DETAILED_NETWORK_TYPE_UNKNOWN]="DETAILED_NETWORK_TYPE_UNKNOWN";var iu={LATENCY_PLAYER_RTSP:7,LATENCY_PLAYER_HTML5_INLINE:6,LATENCY_PLAYER_HTML5_FULLSCREEN:5,LATENCY_PLAYER_HTML5:4,LATENCY_PLAYER_FRAMEWORK:3,LATENCY_PLAYER_FLASH:2,LATENCY_PLAYER_EXO:1,LATENCY_PLAYER_UNKNOWN:0};iu[iu.LATENCY_PLAYER_RTSP]="LATENCY_PLAYER_RTSP";
iu[iu.LATENCY_PLAYER_HTML5_INLINE]="LATENCY_PLAYER_HTML5_INLINE";iu[iu.LATENCY_PLAYER_HTML5_FULLSCREEN]="LATENCY_PLAYER_HTML5_FULLSCREEN";iu[iu.LATENCY_PLAYER_HTML5]="LATENCY_PLAYER_HTML5";iu[iu.LATENCY_PLAYER_FRAMEWORK]="LATENCY_PLAYER_FRAMEWORK";iu[iu.LATENCY_PLAYER_FLASH]="LATENCY_PLAYER_FLASH";iu[iu.LATENCY_PLAYER_EXO]="LATENCY_PLAYER_EXO";iu[iu.LATENCY_PLAYER_UNKNOWN]="LATENCY_PLAYER_UNKNOWN";
var ju={LATENCY_AD_BREAK_TYPE_POSTROLL:3,LATENCY_AD_BREAK_TYPE_MIDROLL:2,LATENCY_AD_BREAK_TYPE_PREROLL:1,LATENCY_AD_BREAK_TYPE_UNKNOWN:0};ju[ju.LATENCY_AD_BREAK_TYPE_POSTROLL]="LATENCY_AD_BREAK_TYPE_POSTROLL";ju[ju.LATENCY_AD_BREAK_TYPE_MIDROLL]="LATENCY_AD_BREAK_TYPE_MIDROLL";ju[ju.LATENCY_AD_BREAK_TYPE_PREROLL]="LATENCY_AD_BREAK_TYPE_PREROLL";ju[ju.LATENCY_AD_BREAK_TYPE_UNKNOWN]="LATENCY_AD_BREAK_TYPE_UNKNOWN";var ku={LATENCY_ACTION_ERROR_STARTUP_TIMEOUT:1,LATENCY_ACTION_ERROR_UNSPECIFIED:0};
ku[ku.LATENCY_ACTION_ERROR_STARTUP_TIMEOUT]="LATENCY_ACTION_ERROR_STARTUP_TIMEOUT";ku[ku.LATENCY_ACTION_ERROR_UNSPECIFIED]="LATENCY_ACTION_ERROR_UNSPECIFIED";var lu={LIVE_STREAM_MODE_WINDOW:5,LIVE_STREAM_MODE_POST:4,LIVE_STREAM_MODE_LP:3,LIVE_STREAM_MODE_LIVE:2,LIVE_STREAM_MODE_DVR:1,LIVE_STREAM_MODE_UNKNOWN:0};lu[lu.LIVE_STREAM_MODE_WINDOW]="LIVE_STREAM_MODE_WINDOW";lu[lu.LIVE_STREAM_MODE_POST]="LIVE_STREAM_MODE_POST";lu[lu.LIVE_STREAM_MODE_LP]="LIVE_STREAM_MODE_LP";
lu[lu.LIVE_STREAM_MODE_LIVE]="LIVE_STREAM_MODE_LIVE";lu[lu.LIVE_STREAM_MODE_DVR]="LIVE_STREAM_MODE_DVR";lu[lu.LIVE_STREAM_MODE_UNKNOWN]="LIVE_STREAM_MODE_UNKNOWN";var mu={VIDEO_STREAM_TYPE_VOD:3,VIDEO_STREAM_TYPE_DVR:2,VIDEO_STREAM_TYPE_LIVE:1,VIDEO_STREAM_TYPE_UNSPECIFIED:0};mu[mu.VIDEO_STREAM_TYPE_VOD]="VIDEO_STREAM_TYPE_VOD";mu[mu.VIDEO_STREAM_TYPE_DVR]="VIDEO_STREAM_TYPE_DVR";mu[mu.VIDEO_STREAM_TYPE_LIVE]="VIDEO_STREAM_TYPE_LIVE";mu[mu.VIDEO_STREAM_TYPE_UNSPECIFIED]="VIDEO_STREAM_TYPE_UNSPECIFIED";
var nu={YT_IDB_TRANSACTION_TYPE_READ:2,YT_IDB_TRANSACTION_TYPE_WRITE:1,YT_IDB_TRANSACTION_TYPE_UNKNOWN:0};nu[nu.YT_IDB_TRANSACTION_TYPE_READ]="YT_IDB_TRANSACTION_TYPE_READ";nu[nu.YT_IDB_TRANSACTION_TYPE_WRITE]="YT_IDB_TRANSACTION_TYPE_WRITE";nu[nu.YT_IDB_TRANSACTION_TYPE_UNKNOWN]="YT_IDB_TRANSACTION_TYPE_UNKNOWN";var ou={PLAYER_ROTATION_TYPE_PORTRAIT_TO_FULLSCREEN:2,PLAYER_ROTATION_TYPE_FULLSCREEN_TO_PORTRAIT:1,PLAYER_ROTATION_TYPE_UNKNOWN:0};ou[ou.PLAYER_ROTATION_TYPE_PORTRAIT_TO_FULLSCREEN]="PLAYER_ROTATION_TYPE_PORTRAIT_TO_FULLSCREEN";
ou[ou.PLAYER_ROTATION_TYPE_FULLSCREEN_TO_PORTRAIT]="PLAYER_ROTATION_TYPE_FULLSCREEN_TO_PORTRAIT";ou[ou.PLAYER_ROTATION_TYPE_UNKNOWN]="PLAYER_ROTATION_TYPE_UNKNOWN";var pu="actionVisualElement spinnerInfo resourceInfo playerInfo commentInfo mdxInfo watchInfo thumbnailLoadInfo creatorInfo unpluggedInfo reelInfo subscriptionsFeedInfo requestIds mediaBrowserActionInfo musicLoadActionInfo shoppingInfo webViewInfo prefetchInfo accelerationSession commerceInfo webInfo tvInfo kabukiInfo mwebInfo musicInfo".split(" ");var qu=z.ytLoggingLatencyUsageStats_||{};A("ytLoggingLatencyUsageStats_",qu);function ru(){this.h=0}
function su(){ru.h||(ru.h=new ru);return ru.h}
ru.prototype.tick=function(a,b,c,d){tu(this,"tick_"+a+"_"+b)||(c={timestamp:c,cttAuthInfo:d},M("web_csi_via_jspb")?(d=new Vj,D(d,1,a),D(d,2,b),a=new Yj,ce(a,Vj,5,Zj,d),Mr(a,c)):lm("latencyActionTicked",{tickName:a,clientActionNonce:b},c))};
ru.prototype.info=function(a,b,c){var d=Object.keys(a).join("");tu(this,"info_"+d+"_"+b)||(a=Object.assign({},a),a.clientActionNonce=b,lm("latencyActionInfo",a,{cttAuthInfo:c}))};
ru.prototype.jspbInfo=function(a,b,c){for(var d="",e=0;e<a.toJSON().length;e++)void 0!==a.toJSON()[e]&&(d=0===e?d.concat(""+e):d.concat("_"+e));tu(this,"info_"+d+"_"+b)||(D(a,2,b),b={cttAuthInfo:c},c=new Yj,ce(c,Rj,7,Zj,a),Mr(c,b))};
ru.prototype.span=function(a,b,c){var d=Object.keys(a).join("");tu(this,"span_"+d+"_"+b)||(a.clientActionNonce=b,lm("latencyActionSpan",a,{cttAuthInfo:c}))};
function tu(a,b){qu[b]=qu[b]||{count:0};var c=qu[b];c.count++;c.time=R();a.h||(a.h=Tl(function(){var d=R(),e;for(e in qu)qu[e]&&6E4<d-qu[e].time&&delete qu[e];a&&(a.h=0)},5E3));
return 5<c.count?(6===c.count&&1>1E5*Math.random()&&(c=new P("CSI data exceeded logging limit with key",b.split("_")),0<=b.indexOf("plev")||ms(c)),!0):!1}
;function uu(){var a=["ol"];Zt("").info.actionType="embed";a&&Kk("TIMING_AFT_KEYS",a);Kk("TIMING_ACTION","embed");if(M("web_csi_via_jspb")){a=L("TIMING_INFO",{});var b=new Rj;a=p(Object.entries(a));for(var c=a.next();!c.done;c=a.next()){var d=p(c.value);c=d.next().value;d=d.next().value;switch(c){case "GetBrowse_rid":var e=new Uj;D(e,1,c);D(e,2,String(d));Tj(b,e);break;case "GetGuide_rid":e=new Uj;D(e,1,c);D(e,2,String(d));Tj(b,e);break;case "GetHome_rid":e=new Uj;D(e,1,c);D(e,2,String(d));Tj(b,e);
break;case "GetPlayer_rid":e=new Uj;D(e,1,c);D(e,2,String(d));Tj(b,e);break;case "GetSearch_rid":e=new Uj;D(e,1,c);D(e,2,String(d));Tj(b,e);break;case "GetSettings_rid":e=new Uj;D(e,1,c);D(e,2,String(d));Tj(b,e);break;case "GetTrending_rid":e=new Uj;D(e,1,c);D(e,2,String(d));Tj(b,e);break;case "GetWatchNext_rid":e=new Uj;D(e,1,c);D(e,2,String(d));Tj(b,e);break;case "yt_red":D(b,14,!!d);break;case "yt_ad":D(b,9,!!d)}}vu(b);b=new Rj;b=D(b,25,!0);b=D(b,1,W[fu(L("TIMING_ACTION"))]);(a=L("PREVIOUS_ACTION"))&&
D(b,13,W[fu(a)]);(a=L("CLIENT_PROTOCOL"))&&D(b,33,a);(a=L("CLIENT_TRANSPORT"))&&D(b,34,a);(a=Es())&&"UNDEFINED_CSN"!==a&&D(b,4,a);a=wu();1!==a&&-1!==a||D(b,6,!0);a=Pt();D(b,3,"cold");xu(a);a=yu();if(0<a.length)for(a=p(a),c=a.next();!c.done;c=a.next())c=c.value,d=new Qj,D(d,1,c),ee(b,83,Qj,d);vu(b)}else{a=L("TIMING_INFO",{});for(b in a)a.hasOwnProperty(b)&&zu(b,a[b]);b={isNavigation:!0,actionType:fu(L("TIMING_ACTION"))};if(a=L("PREVIOUS_ACTION"))b.previousAction=fu(a);if(a=L("CLIENT_PROTOCOL"))b.httpProtocol=
a;if(a=L("CLIENT_TRANSPORT"))b.transportProtocol=a;(a=Es())&&"UNDEFINED_CSN"!==a&&(b.clientScreenNonce=a);a=wu();if(1===a||-1===a)b.isVisible=!0;Pt();b.loadType="cold";xu(Pt());a=yu();if(0<a.length)for(b.resourceInfo=[],a=p(a),c=a.next();!c.done;c=a.next())b.resourceInfo.push({resourceCache:c.value});Au(b)}b=Pt();a=Tt();if(!(M("skip_setting_info_in_csi_data_object")&&"cold"!==Qt().loadType||"cold"!==b.yt_lt&&"cold"!==a.loadType)){c=Rt();d=St();d=d.gelTicks?d.gelTicks:d.gelTicks={};for(var f in c)if(!(f in
d))if("number"===typeof c[f])Z(f,Kt(f));else if(M("log_repeated_ytcsi_ticks")){e=p(c[f]);for(var g=e.next();!g.done;g=e.next())Z(f.slice(1),g.value)}f={};c=!1;d=p(Object.keys(b));for(e=d.next();!e.done;e=d.next())e=e.value,(e=gu(e,b[e]))&&!Wt(Tt(),e)&&(xt(a,e),xt(f,e),c=!0);c&&Au(f)}A("ytglobal.timingready_",!0);f=L("TIMING_ACTION");B("ytglobal.timingready_")&&f&&"_start"in Rt()&&Jt()&&Vt()}
function zu(a,b,c,d){if(null!==b){var e=Pt(c);M("skip_setting_info_in_csi_data_object")?"yt_lt"===a&&(e="string"===typeof b?b:""+b,Qt(c).loadType=e):e[a]=b;(a=gu(a,b,c))&&Au(a,c,d)}}
function Au(a,b,c){if(!M("web_csi_via_jspb")||(void 0===c?0:c))c=Zt(b||""),xt(c.info,a),M("skip_setting_info_in_csi_data_object")&&a.loadType&&(c=a.loadType,Qt(b).loadType=c),xt(Tt(b),a),c=Ut(b),b=Ot(b).cttAuthInfo,su().info(a,c,b);else{c=new Rj;var d=Object.keys(a);a=Object.values(a);for(var e=0;e<d.length;e++){var f=d[e];try{switch(f){case "actionType":D(c,1,W[a[e]]);break;case "clientActionNonce":D(c,2,a[e]);break;case "clientScreenNonce":D(c,4,a[e]);break;case "loadType":D(c,3,a[e]);break;case "isPrewarmedLaunch":D(c,
92,a[e]);break;case "isFirstInstall":D(c,55,a[e]);break;case "networkType":D(c,5,hu[a[e]]);break;case "connectionType":D(c,26,X[a[e]]);break;case "detailedConnectionType":D(c,27,Y[a[e]]);break;case "isVisible":D(c,6,a[e]);break;case "playerType":D(c,7,iu[a[e]]);break;case "clientPlaybackNonce":D(c,8,a[e]);break;case "adClientPlaybackNonce":D(c,28,a[e]);break;case "previousCpn":D(c,77,a[e]);break;case "targetCpn":D(c,76,a[e]);break;case "isMonetized":D(c,9,a[e]);break;case "isPrerollAllowed":D(c,16,
a[e]);break;case "isPrerollShown":D(c,17,a[e]);break;case "adType":D(c,12,a[e]);break;case "adTypesAllowed":D(c,36,a[e]);break;case "adNetworks":D(c,37,a[e]);break;case "previousAction":D(c,13,W[a[e]]);break;case "isRedSubscriber":D(c,14,a[e]);break;case "serverTimeMs":D(c,15,a[e]);break;case "videoId":c.setVideoId(a[e]);break;case "adVideoId":D(c,20,a[e]);break;case "targetVideoId":D(c,78,a[e]);break;case "adBreakType":D(c,21,ju[a[e]]);break;case "isNavigation":D(c,25,a[e]);break;case "viewportHeight":D(c,
29,a[e]);break;case "viewportWidth":D(c,30,a[e]);break;case "screenHeight":D(c,84,a[e]);break;case "screenWidth":D(c,85,a[e]);break;case "browseId":D(c,31,a[e]);break;case "isCacheHit":D(c,32,a[e]);break;case "httpProtocol":D(c,33,a[e]);break;case "transportProtocol":D(c,34,a[e]);break;case "searchQuery":D(c,41,a[e]);break;case "isContinuation":D(c,42,a[e]);break;case "availableProcessors":D(c,43,a[e]);break;case "sdk":D(c,44,a[e]);break;case "isLocalStream":D(c,45,a[e]);break;case "navigationRequestedSameUrl":D(c,
64,a[e]);break;case "shellStartupDurationMs":D(c,70,a[e]);break;case "appInstallDataAgeMs":D(c,73,a[e]);break;case "latencyActionError":D(c,71,ku[a[e]]);break;case "actionStep":D(c,79,a[e]);break;case "jsHeapSizeLimit":D(c,80,a[e]);break;case "totalJsHeapSize":D(c,81,a[e]);break;case "usedJsHeapSize":D(c,82,a[e]);break;case "sourceVideoDurationMs":D(c,90,a[e]);break;case "videoOutputFrames":D(c,93,a[e]);break;case "isResume":D(c,104,a[e]);break;case "adPrebufferedTimeSecs":D(c,39,a[e]);break;case "isLivestream":D(c,
47,a[e]);break;case "liveStreamMode":D(c,91,lu[a[e]]);break;case "adCpn2":D(c,48,a[e]);break;case "adDaiDriftMillis":D(c,49,a[e]);break;case "videoStreamType":D(c,53,mu[a[e]]);break;case "playbackRequiresTap":D(c,56,a[e]);break;case "performanceNavigationTiming":D(c,67,a[e]);break;case "transactionType":D(c,74,nu[a[e]]);break;case "playerRotationType":D(c,101,ou[a[e]]);break;case "allowedPreroll":D(c,10,a[e]);break;case "shownPreroll":D(c,11,a[e]);break;case "getHomeRequestId":D(c,57,a[e]);break;
case "getSearchRequestId":D(c,60,a[e]);break;case "getPlayerRequestId":D(c,61,a[e]);break;case "getWatchNextRequestId":D(c,62,a[e]);break;case "getBrowseRequestId":D(c,63,a[e]);break;case "getLibraryRequestId":D(c,66,a[e]);break;default:pu.includes(f)&&Tk(new P("Codegen laipb translator asked to translate message field",""+f))}}catch(g){Tk(Error("Codegen laipb translator failed to set "+f))}}vu(c,b)}}
function vu(a,b){if(M("skip_setting_info_in_csi_data_object")){var c=ge(Td(a,3),"");c&&(Qt(b).loadType=c)}else c=St(b),c.jspbInfos||(c.jspbInfos=[]),c.jspbInfos.push(me(a));Zt(b||"").jspbInfo.push(a);c=Ut(b);b=Ot(b).cttAuthInfo;su().jspbInfo(a,c,b)}
function Z(a,b,c){if(!b&&"_"!==a[0]){var d=a;S.mark&&(0==d.lastIndexOf("mark_",0)||(d="mark_"+d),c&&(d+=" ("+c+")"),S.mark(d))}d=Zt(c||"");d.tick[a]=b||R();if(d.callback&&d.callback[a]){d=p(d.callback[a]);for(var e=d.next();!e.done;e=d.next())e=e.value,e()}d=St(c);d.gelTicks&&(d.gelTicks[a]=!0);e=Rt(c);d=b||R();M("log_repeated_ytcsi_ticks")?a in e||(e[a]=d):e[a]=d;e=Ut(c);var f=Ot(c).cttAuthInfo;"_start"===a?(a=su(),tu(a,"baseline_"+e)||(b={timestamp:b,cttAuthInfo:f},M("web_csi_via_jspb")?(a=new Pj,
D(a,1,e),e=new Yj,ce(e,Pj,6,Zj,a),Mr(e,b)):lm("latencyActionBaselined",{clientActionNonce:e},b))):su().tick(a,e,b,f);Vt(c);return d}
function Bu(){var a=Ut();requestAnimationFrame(function(){setTimeout(function(){a===Ut()&&Z("ol",void 0,void 0)},0)})}
function wu(){var a=document;if("visibilityState"in a)a=a.visibilityState;else{var b=hq+"VisibilityState";a=b in a?a[b]:void 0}switch(a){case "hidden":return 0;case "visible":return 1;case "prerender":return 2;case "unloaded":return 3;default:return-1}}
function xu(a){var b=Lt(),c=Nt(),d=L("CSI_START_TIMESTAMP_MILLIS",0);0<d&&!M("embeds_web_enable_csi_start_override_killswitch")&&(c=d);c&&(Z("srt",b.responseStart),1!==a.prerender&&Z("_start",c,void 0));a=Xt();0<a&&Z("fpt",a);a=Lt();a.isPerformanceNavigationTiming&&Au({performanceNavigationTiming:!0});Z("nreqs",a.requestStart,void 0);Z("nress",a.responseStart,void 0);Z("nrese",a.responseEnd,void 0);0<a.redirectEnd-a.redirectStart&&(Z("nrs",a.redirectStart,void 0),Z("nre",a.redirectEnd,void 0));0<
a.domainLookupEnd-a.domainLookupStart&&(Z("ndnss",a.domainLookupStart,void 0),Z("ndnse",a.domainLookupEnd,void 0));0<a.connectEnd-a.connectStart&&(Z("ntcps",a.connectStart,void 0),Z("ntcpe",a.connectEnd,void 0));a.secureConnectionStart>=Nt()&&0<a.connectEnd-a.secureConnectionStart&&(Z("nstcps",a.secureConnectionStart,void 0),Z("ntcpe",a.connectEnd,void 0));S&&"getEntriesByType"in S&&Cu()}
function Du(a,b){a=document.querySelector(a);if(!a)return!1;var c="",d=a.nodeName;"SCRIPT"===d?(c=a.src,c||(c=a.getAttribute("data-timing-href"))&&(c=window.location.protocol+c)):"LINK"===d&&(c=a.href);hc()&&a.setAttribute("nonce",hc());return c?(a=S.getEntriesByName(c))&&a[0]&&(a=a[0],c=Nt(),Z("rsf_"+b,c+Math.round(a.fetchStart)),Z("rse_"+b,c+Math.round(a.responseEnd)),void 0!==a.transferSize&&0===a.transferSize)?!0:!1:!1}
function yu(){var a=[];if(document.querySelector&&S&&S.getEntriesByName)for(var b in It)if(It.hasOwnProperty(b)){var c=It[b];Du(b,c)&&a.push(c)}return a}
function Cu(){var a=window.location.protocol,b=S.getEntriesByType("resource");b=gb(b,function(c){return 0===c.name.indexOf(a+"//fonts.gstatic.com/s/")});
(b=ib(b,function(c,d){return d.duration>c.duration?d:c},{duration:0}))&&0<b.startTime&&0<b.responseEnd&&(Z("wffs",Mt(b.startTime)),Z("wffe",Mt(b.responseEnd)))}
var Eu=window;Eu.ytcsi&&(Eu.ytcsi.info=zu,Eu.ytcsi.tick=Z);var Fu="tokens consistency mss client_location entities response_received_commands store PLAYER_PRELOAD".split(" "),Gu=["type.googleapis.com/youtube.api.pfiinnertube.YoutubeApiInnertube.BrowseResponse"];function Hu(a,b,c,d){this.m=a;this.L=b;this.l=c;this.j=d;this.i=void 0;this.h=new Map;a.Qa||(a.Qa={});a.Qa=Object.assign({},wt,a.Qa)}
function Iu(a,b,c,d){if(void 0!==Hu.h){if(d=Hu.h,a=[a!==d.m,b!==d.L,c!==d.l,!1,!1,void 0!==d.i],a.some(function(e){return e}))throw new P("InnerTubeTransportService is already initialized",a);
}else Hu.h=new Hu(a,b,c,d)}
function Ju(a){var b={signalServiceEndpoint:{signal:"GET_DATASYNC_IDS"}};var c=void 0===c?et:c;var d=ot(b,a.m);if(!d)return Hf(new P("Error: No request builder found for command.",b));var e=d.m(b,void 0,c);return e?new Cf(function(f){var g,h,k;return x(function(m){if(1==m.h){h="cors"===(null==(g=e.xa)?void 0:g.mode)?"cors":void 0;if(a.l.wd){var q=e.config,r;q=null==q?void 0:null==(r=q.ab)?void 0:r.sessionIndex;r=dt({sessionIndex:q});k=Object.assign({},Ku(h),r);return m.u(2)}return v(m,Lu(e.config,
h),3)}2!=m.h&&(k=m.i);f(Mu(a,e,k));m.h=0})}):Hf(new P("Error: Failed to build request for command.",b))}
function Nu(a,b,c){var d;if(b&&!(null==b?0:null==(d=b.Er)?0:d.Hr)&&a.j){d=p(Fu);for(var e=d.next();!e.done;e=d.next())e=e.value,a.j[e]&&a.j[e].handleResponse(b,c)}}
function Mu(a,b,c){var d,e,f,g,h,k,m,q,r,w,t,y,E,F,O,N,Q,da,U,qb,Wa,oa,I,ma,fa,Ce,De,rd;return x(function(qa){switch(qa.h){case 1:qa.u(2);break;case 3:if((d=qa.i)&&!d.isExpired())return qa.return(Promise.resolve(d.h()));case 2:if(null==(e=b)?0:null==(f=e.ga)?0:f.context)for(g=p([]),h=g.next();!h.done;h=g.next())k=h.value,k.Ar(b.ga.context);if(null==(m=a.i)||!m.Fr(b.input,b.ga)){qa.u(4);break}return v(qa,a.i.vr(b.input,b.ga),5);case 5:return q=qa.i,M("kevlar_process_local_innertube_responses_killswitch")||
Nu(a,q,b),qa.return(q);case 4:return(t=null==(w=b.config)?void 0:w.wa)&&a.h.has(t)&&M("web_memoize_inflight_requests")?r=a.h.get(t):(y=JSON.stringify(b.ga),O=null!=(F=null==(E=b.xa)?void 0:E.headers)?F:{},b.xa=Object.assign({},b.xa,{headers:Object.assign({},O,c)}),N=Object.assign({},b.xa),"POST"===b.xa.method&&(N=Object.assign({},N,{body:y})),(null==(Q=b.config)?0:Q.jd)&&Z(b.config.jd),da=function(){return a.L.fetch(b.input,N,b.config)},r=da(),t&&a.h.set(t,r)),v(qa,r,6);
case 6:U=qa.i;if(M("web_one_platform_error_handling")&&U&&"error"in U&&(null==(qb=U)?0:null==(Wa=qb.error)?0:Wa.details))for(oa=U.error.details,I=p(oa),ma=I.next();!ma.done;ma=I.next())fa=ma.value,(Ce=fa["@type"])&&-1<Gu.indexOf(Ce)&&(delete fa["@type"],U=fa);t&&a.h.has(t)&&a.h.delete(t);(null==(De=b.config)?0:De.kd)&&Z(b.config.kd);if(U||null==(rd=a.i)||!rd.lr(b.input,b.ga)){qa.u(7);break}return v(qa,a.i.ur(b.input,b.ga),8);case 8:U=qa.i;case 7:return Nu(a,U,b),qa.return(U||void 0)}})}
function Lu(a,b){var c,d,e,f;return x(function(g){if(1==g.h){e=null==(c=a)?void 0:null==(d=c.ab)?void 0:d.sessionIndex;var h=dt({sessionIndex:e});if(!(h instanceof Cf)){var k=new Cf(cb);Df(k,2,h);h=k}return v(g,h,2)}f=g.i;return g.return(Promise.resolve(Object.assign({},Ku(b),f)))})}
function Ku(a){var b={"Content-Type":"application/json"};L("EOM_VISITOR_DATA")?b["X-Goog-EOM-Visitor-Id"]=L("EOM_VISITOR_DATA"):L("VISITOR_DATA")&&(b["X-Goog-Visitor-Id"]=L("VISITOR_DATA"));M("track_webfe_innertube_auth_mismatch")&&(b["X-Youtube-Bootstrap-Logged-In"]=L("LOGGED_IN",!1));"cors"!==a&&((a=L("INNERTUBE_CONTEXT_CLIENT_NAME"))&&(b["X-Youtube-Client-Name"]=a),(a=L("INNERTUBE_CONTEXT_CLIENT_VERSION"))&&(b["X-Youtube-Client-Version"]=a),(a=L("CHROME_CONNECTED_HEADER"))&&(b["X-Youtube-Chrome-Connected"]=
a),(a=L("DOMAIN_ADMIN_STATE"))&&(b["X-Youtube-Domain-Admin-State"]=a));return b}
;var Ou=new Iq("INNERTUBE_TRANSPORT_TOKEN");var Pu=["share/get_web_player_share_panel"],Qu=["feedback"],Ru=["notification/modify_channel_preference"],Su=["browse/edit_playlist"],Tu=["subscription/subscribe"],Uu=["subscription/unsubscribe"];function Vu(){}
u(Vu,tt);Vu.prototype.j=function(){return Tu};
Vu.prototype.h=function(a){return Uq(a,Ek)||void 0};
Vu.prototype.i=function(a,b,c){c=void 0===c?{}:c;b.channelIds&&(a.channelIds=b.channelIds);b.siloName&&(a.siloName=b.siloName);b.params&&(a.params=b.params);c.botguardResponse&&(a.botguardResponse=c.botguardResponse);c.feature&&(a.clientFeature=c.feature)};
ea.Object.defineProperties(Vu.prototype,{l:{configurable:!0,enumerable:!0,get:function(){return!0}}});function Wu(){}
u(Wu,tt);Wu.prototype.j=function(){return Uu};
Wu.prototype.h=function(a){return Uq(a,Dk)||void 0};
Wu.prototype.i=function(a,b){b.channelIds&&(a.channelIds=b.channelIds);b.siloName&&(a.siloName=b.siloName);b.params&&(a.params=b.params)};
ea.Object.defineProperties(Wu.prototype,{l:{configurable:!0,enumerable:!0,get:function(){return!0}}});function Xu(){}
u(Xu,tt);Xu.prototype.j=function(){return Qu};
Xu.prototype.h=function(a){return Uq(a,nj)||void 0};
Xu.prototype.i=function(a,b,c){a.feedbackTokens=[];b.feedbackToken&&a.feedbackTokens.push(b.feedbackToken);if(b=b.cpn||c.cpn)a.feedbackContext={cpn:b};a.isFeedbackTokenUnencrypted=!!c.is_feedback_token_unencrypted;a.shouldMerge=!1;c.extra_feedback_tokens&&(a.shouldMerge=!0,a.feedbackTokens=a.feedbackTokens.concat(c.extra_feedback_tokens))};
ea.Object.defineProperties(Xu.prototype,{l:{configurable:!0,enumerable:!0,get:function(){return!0}}});function Yu(){}
u(Yu,tt);Yu.prototype.j=function(){return Ru};
Yu.prototype.h=function(a){return Uq(a,Ck)||void 0};
Yu.prototype.i=function(a,b){b.params&&(a.params=b.params);b.secondaryParams&&(a.secondaryParams=b.secondaryParams)};function Zu(){}
u(Zu,tt);Zu.prototype.j=function(){return Su};
Zu.prototype.h=function(a){return Uq(a,Bk)||void 0};
Zu.prototype.i=function(a,b){b.actions&&(a.actions=b.actions);b.params&&(a.params=b.params);b.playlistId&&(a.playlistId=b.playlistId)};function $u(){}
u($u,tt);$u.prototype.j=function(){return Pu};
$u.prototype.h=function(a){return Uq(a,Ak)};
$u.prototype.i=function(a,b,c){c=void 0===c?{}:c;b.serializedShareEntity&&(a.serializedSharedEntity=b.serializedShareEntity);c.includeListId&&(a.includeListId=!0)};var Kq=new Iq("NETWORK_SLI_TOKEN");function av(a){this.h=a}
av.prototype.fetch=function(a,b){var c=this,d,e;return x(function(f){c.h&&(d=lc(mc(5,Cc(a,"key")))||"/UNKNOWN_PATH",c.h.start(d));e=new window.Request(a,b);return M("web_fetch_promise_cleanup_killswitch")?f.return(Promise.resolve(fetch(e).then(function(g){return c.handleResponse(g)}).catch(function(g){ms(g)}))):f.return(fetch(e).then(function(g){return c.handleResponse(g)}).catch(function(g){ms(g)}))})};
av.prototype.handleResponse=function(a){var b=a.text().then(function(c){return JSON.parse(c.replace(")]}'",""))});
a.redirected||a.ok?this.h&&this.h.success():(this.h&&this.h.rr(),b=b.then(function(c){ms(new P("Error: API fetch failed",a.status,a.url,c));return Object.assign({},c,{errorMetadata:{status:a.status}})}));
return b};
av[Hq]=[new Jq];var bv=new Iq("NETWORK_MANAGER_TOKEN");var cv;function dv(a){xo.call(this,1,arguments);this.csn=a}
u(dv,xo);var Go=new yo("screen-created",dv),ev=[],gv=fv,hv=0;function iv(a,b,c,d,e,f,g){function h(){ms(new P("newScreen() parent element does not have a VE - rootVe",b))}
var k=gv(),m=new xs({veType:b,youtubeData:f,jspbYoutubeData:void 0});f={Z:k};e&&(f.cttAuthInfo=e);M("il_via_jspb")?(e=new Ej,e.i(k),Fj(e,m.getAsJspb()),c&&c.visualElement?(m=new Gj,c.clientScreenNonce&&D(m,2,c.clientScreenNonce),Hj(m,c.visualElement.getAsJspb()),g&&D(m,4,ak[g]),G(e,Gj,5,m)):c&&h(),d&&D(e,3,d),Rr(e,f,a)):(e={csn:k,pageVe:m.getAsJson()},c&&c.visualElement?(e.implicitGesture={parentCsn:c.clientScreenNonce,gesturedVe:c.visualElement.getAsJson()},g&&(e.implicitGesture.gestureType=g)):
c&&h(),d&&(e.cloneCsn=d),a?Gr("screenCreated",e,a,f):lm("screenCreated",e,f));Do(Go,new dv(k));return k}
function jv(a,b,c,d){var e=d.filter(function(k){k.csn!==b?(k.csn=b,k=!0):k=!1;return k}),f={cttAuthInfo:Gs(b)||void 0,
Z:b};d=p(d);for(var g=d.next();!g.done;g=d.next())g=g.value.getAsJson(),(pb(g)||!g.trackingParams&&!g.veType)&&ms(Error("Child VE logged with no data"));if(M("il_via_jspb")){var h=new Ij;h.i(b);Kj(h,c.getAsJspb());hb(e,function(k){k=k.getAsJspb();ee(h,3,Cj,k)});
"UNDEFINED_CSN"===b?kv("visualElementAttached",f,void 0,h):Sr(h,f,a)}else c={csn:b,parentVe:c.getAsJson(),childVes:hb(e,function(k){return k.getAsJson()})},"UNDEFINED_CSN"===b?kv("visualElementAttached",f,c):a?Gr("visualElementAttached",c,a,f):lm("visualElementAttached",c,f)}
function fv(){for(var a=Math.random()+"",b=[],c=0,d=0;d<a.length;d++){var e=a.charCodeAt(d);255<e&&(b[c++]=e&255,e>>=8);b[c++]=e}return ad(b,3)}
function kv(a,b,c,d){ev.push({Eb:a,payload:c,ma:d,options:b});hv||(hv=Ho())}
function Io(a){if(ev){for(var b=p(ev),c=b.next();!c.done;c=b.next())if(c=c.value,M("il_via_jspb")&&c.ma)switch(c.ma.i(a.csn),c.Eb){case "screenCreated":Rr(c.ma,c.options);break;case "visualElementAttached":Sr(c.ma,c.options);break;case "visualElementShown":Nr(c.ma,c.options);break;case "visualElementHidden":Or(c.ma,c.options);break;case "visualElementGestured":Pr(c.ma,c.options);break;case "visualElementStateChanged":Qr(c.ma,c.options);break;default:ms(new P("flushQueue unable to map payloadName to JSPB setter"))}else c.payload&&
(c.payload.csn=a.csn,Gr(c.Eb,c.payload,null,c.options));ev.length=0}hv=0}
;function lv(){this.i=new Set;this.h=new Set;this.j=new Map;this.client=fq;this.csn=null}
function mv(){lv.h||(lv.h=new lv);return lv.h}
lv.prototype.m=function(a){this.client=a};
lv.prototype.clear=function(){this.i.clear();this.h.clear();this.j.clear();this.csn=null};function nv(){this.i=new Set;this.h=new Set;this.j=new Map}
nv.prototype.m=function(a){M("use_ts_visibilitylogger")&&mv().m(a)};
nv.prototype.clear=function(){M("use_ts_visibilitylogger")?mv().clear():(this.i.clear(),this.h.clear(),this.j.clear())};
Na(nv);function ov(){this.o=[];this.v=[];this.h=[];this.l=[];this.M=[];this.i=new Set;this.s=new Map}
ov.prototype.m=function(a){this.client=a};
function pv(a,b,c){c=void 0===c?0:c;b.then(function(d){a.i.has(c)&&a.j&&a.j();var e=Es(c),f=Cs(c);if(e&&f){var g;(null==d?0:null==(g=d.response)?0:g.trackingParams)&&jv(a.client,e,f,[ys(d.response.trackingParams)]);var h;(null==d?0:null==(h=d.playerResponse)?0:h.trackingParams)&&jv(a.client,e,f,[ys(d.playerResponse.trackingParams)])}})}
function qv(a,b,c,d){d=void 0===d?0:d;if(a.i.has(d))a.o.push([b,c]);else{var e=Es(d);c=c||Cs(d);e&&c&&jv(a.client,e,c,[b])}}
ov.prototype.clickCommand=function(a,b,c){var d=a.clickTrackingParams;c=void 0===c?0:c;if(d)if(c=Es(void 0===c?0:c)){a=this.client;var e=ys(d);d={cttAuthInfo:Gs(c)||void 0,Z:c};M("il_via_jspb")?(b=new Lj,b.i(c),e=e.getAsJspb(),G(b,Cj,2,e),D(b,4,ak.INTERACTION_LOGGING_GESTURE_TYPE_GENERIC_CLICK),"UNDEFINED_CSN"===c?kv("visualElementGestured",d,void 0,b):Pr(b,d,a)):(e={csn:c,ve:e.getAsJson(),gestureType:"INTERACTION_LOGGING_GESTURE_TYPE_GENERIC_CLICK"},b&&(e.clientData=b),"UNDEFINED_CSN"===c?kv("visualElementGestured",
d,e):a?Gr("visualElementGestured",e,a,d):lm("visualElementGestured",e,d));b=!0}else b=!1;else b=!1;return b};
ov.prototype.visualElementStateChanged=function(a,b,c){c=void 0===c?0:c;0===c&&this.i.has(c)?this.v.push([a,b]):rv(this,a,b,c)};
function rv(a,b,c,d){d=void 0===d?0:d;var e=Es(d);d=b||Cs(d);e&&d&&(a=a.client,b={cttAuthInfo:Gs(e)||void 0,Z:e},M("il_via_jspb")?(c=new Oj,c.i(e),d=d.getAsJspb(),G(c,Cj,2,d),"UNDEFINED_CSN"===e?kv("visualElementStateChanged",b,void 0,c):Qr(c,b,a)):(c={csn:e,ve:d.getAsJson(),clientData:c},"UNDEFINED_CSN"===e?kv("visualElementStateChanged",b,c):a?Gr("visualElementStateChanged",c,a,b):lm("visualElementStateChanged",c,b)))}
function sv(a,b,c){c=void 0===c?{}:c;a.i.add(c.layer||0);a.j=function(){tv(a,b,c);var f=Cs(c.layer);if(f){for(var g=p(a.o),h=g.next();!h.done;h=g.next())h=h.value,qv(a,h[0],h[1]||f,c.layer);f=p(a.v);for(g=f.next();!g.done;g=f.next())g=g.value,rv(a,g[0],g[1])}};
Es(c.layer)||a.j();if(c.Wb)for(var d=p(c.Wb),e=d.next();!e.done;e=d.next())pv(a,e.value,c.layer);else ls(Error("Delayed screen needs a data promise."))}
function tv(a,b,c){c=void 0===c?{}:c;c.layer||(c.layer=0);var d=void 0!==c.dd?c.dd:c.layer;var e=Es(d);d=Cs(d);var f;d&&(void 0!==c.parentCsn?f={clientScreenNonce:c.parentCsn,visualElement:d}:e&&"UNDEFINED_CSN"!==e&&(f={clientScreenNonce:e,visualElement:d}));var g,h=L("EVENT_ID");"UNDEFINED_CSN"===e&&h&&(g={servletData:{serializedServletEventId:h}});try{var k=iv(a.client,b,f,c.Vb,c.cttAuthInfo,g,c.sr)}catch(m){os(m,{Cr:b,rootVe:d,zr:void 0,mr:e,yr:f,Vb:c.Vb});ls(m);return}Hs(k,b,c.layer,c.cttAuthInfo);
if(b=e&&"UNDEFINED_CSN"!==e&&d){a:{b=p(Object.values(ws));for(f=b.next();!f.done;f=b.next())if(Es(f.value)===e){b=!0;break a}b=!1}b=!b}b&&(b=a.client,g=!0,h=(g=void 0===g?!1:g)?16:8,f={cttAuthInfo:Gs(e)||void 0,Z:e,Xb:g},M("il_via_jspb")?(h=new Mj,h.i(e),d=d.getAsJspb(),G(h,Cj,2,d),D(h,4,g?16:8),"UNDEFINED_CSN"===e?kv("visualElementHidden",f,void 0,h):Or(h,f,b)):(d={csn:e,ve:d.getAsJson(),eventType:h},"UNDEFINED_CSN"===e?kv("visualElementHidden",f,d):b?Gr("visualElementHidden",d,b,f):lm("visualElementHidden",
d,f)));a.h[a.h.length-1]&&!a.h[a.h.length-1].csn&&(a.h[a.h.length-1].csn=k||"");Au({clientScreenNonce:k});d=nv.getInstance();M("use_ts_visibilitylogger")?(d=mv(),d.clear(),d.csn=Es()):d.clear();d=Cs(c.layer);e&&"UNDEFINED_CSN"!==e&&d&&(M("web_mark_root_visible")||M("music_web_mark_root_visible"))&&(e=k,k={cttAuthInfo:Gs(e)||void 0,Z:e},M("il_via_jspb")?(b=new Nj,b.i(e),f=d.getAsJspb(),G(b,Cj,2,f),D(b,4,1),"UNDEFINED_CSN"===e?kv("visualElementShown",k,void 0,b):Nr(b,k)):(b={csn:e,ve:d.getAsJson(),
eventType:1},"UNDEFINED_CSN"===e?kv("visualElementShown",k,b):lm("visualElementShown",b,k)));a.i.delete(c.layer||0);a.j=void 0;e=p(a.s);for(k=e.next();!k.done;k=e.next())b=p(k.value),k=b.next().value,b=b.next().value,b.has(c.layer)&&d&&qv(a,k,d,c.layer);for(c=0;c<a.l.length;c++){e=a.l[c];try{e()}catch(m){ls(m)}}for(c=a.l.length=0;c<a.M.length;c++){e=a.M[c];try{e()}catch(m){ls(m)}}}
;function uv(){var a,b,c;return x(function(d){if(1==d.h)return a=Pq().resolve(Ou),a?v(d,Ju(a),2):(ms(Error("InnertubeTransportService unavailable in fetchDatasyncIds")),d.return(void 0));if(b=d.i){if(b.errorMetadata)return ms(Error("Datasync IDs fetch responded with "+b.errorMetadata.status+": "+b.error)),d.return(void 0);c=b.pr;return d.return(c)}ms(Error("Network request to get Datasync IDs failed."));return d.return(void 0)})}
;var vv=z.caches,wv;function xv(a){var b=a.indexOf(":");return-1===b?{jc:a}:{jc:a.substring(0,b),datasyncId:a.substring(b+1)}}
function yv(){return x(function(a){if(void 0!==wv)return a.return(wv);wv=new Promise(function(b){var c;return x(function(d){switch(d.h){case 1:return ya(d,2),v(d,vv.open("test-only"),4);case 4:return v(d,vv.delete("test-only"),5);case 5:za(d,3);break;case 2:if(c=Aa(d),c instanceof Error&&"SecurityError"===c.name)return b(!1),d.return();case 3:b("caches"in window),d.h=0}})});
return a.return(wv)})}
function zv(a){var b,c,d,e,f,g,h;x(function(k){if(1==k.h)return v(k,yv(),2);if(3!=k.h){if(!k.i)return k.return(!1);b=[];return v(k,vv.keys(),3)}c=k.i;d=p(c);for(e=d.next();!e.done;e=d.next())f=e.value,g=xv(f),h=g.datasyncId,!h||a.includes(h)||b.push(vv.delete(f));return k.return(Promise.all(b).then(function(m){return m.some(function(q){return q})}))})}
function Av(){var a,b,c,d,e,f,g;return x(function(h){if(1==h.h)return v(h,yv(),2);if(3!=h.h){if(!h.i)return h.return(!1);a=$l("cache contains other");return v(h,vv.keys(),3)}b=h.i;c=p(b);for(d=c.next();!d.done;d=c.next())if(e=d.value,f=xv(e),(g=f.datasyncId)&&g!==a)return h.return(!0);return h.return(!1)})}
;function Bv(){try{return!!self.localStorage}catch(a){return!1}}
;function Cv(a){a=a.match(/(.*)::.*::.*/);if(null!==a)return a[1]}
function Dv(a){if(Bv()){var b=Object.keys(window.localStorage);b=p(b);for(var c=b.next();!c.done;c=b.next()){c=c.value;var d=Cv(c);void 0===d||a.includes(d)||self.localStorage.removeItem(c)}}}
function Ev(){if(!Bv())return!1;var a=$l(),b=Object.keys(window.localStorage);b=p(b);for(var c=b.next();!c.done;c=b.next())if(c=Cv(c.value),void 0!==c&&c!==a)return!0;return!1}
;function Fv(){uv().then(function(a){a&&(An(a),zv(a),Dv(a))})}
function Gv(){var a=new jp;bi.R(function(){var b,c,d,e;return x(function(f){switch(f.h){case 1:if(M("ytidb_clear_optimizations_killswitch")){f.u(2);break}b=$l("clear");if(b.startsWith("V")){var g=[b];An(g);zv(g);Dv(g);return f.return()}c=Ev();return v(f,Av(),3);case 3:return d=f.i,v(f,Bn(),4);case 4:if(e=f.i,!c&&!d&&!e)return f.return();case 2:a.U()?Fv():a.l.add("publicytnetworkstatus-online",Fv,!0,void 0,void 0),f.h=0}})})}
;function Hv(a){a&&(a.dataset?a.dataset[Iv("loaded")]="true":a.setAttribute("data-loaded","true"))}
function Jv(a,b){return a?a.dataset?a.dataset[Iv(b)]:a.getAttribute("data-"+b):null}
var Kv={};function Iv(a){return Kv[a]||(Kv[a]=String(a).replace(/\-([a-z])/g,function(b,c){return c.toUpperCase()}))}
;var Lv=/\.vflset|-vfl[a-zA-Z0-9_+=-]+/,Mv=/-[a-zA-Z]{2,3}_[a-zA-Z]{2,3}(?=(\/|$))/;function Nv(a,b,c){c=void 0===c?null:c;if(window.spf&&spf.script){c="";if(a){var d=a.indexOf("jsbin/"),e=a.lastIndexOf(".js"),f=d+6;-1<d&&-1<e&&e>f&&(c=a.substring(f,e),c=c.replace(Lv,""),c=c.replace(Mv,""),c=c.replace("debug-",""),c=c.replace("tracing-",""))}spf.script.load(a,c,b)}else Ov(a,b,c)}
function Ov(a,b,c){c=void 0===c?null:c;var d=Pv(a),e=document.getElementById(d),f=e&&Jv(e,"loaded"),g=e&&!f;f?b&&b():(b&&(f=Bq(d,b),b=""+Ra(b),Qv[b]=f),g||(e=Rv(a,d,function(){Jv(e,"loaded")||(Hv(e),Eq(d),ml(Ya(Fq,d),0))},c)))}
function Rv(a,b,c,d){d=void 0===d?null:d;var e=pf("SCRIPT");e.id=b;e.onload=function(){c&&setTimeout(c,0)};
e.onreadystatechange=function(){switch(e.readyState){case "loaded":case "complete":e.onload()}};
d&&e.setAttribute("nonce",d);Lh(e,Ib(a));a=document.getElementsByTagName("head")[0]||document.body;a.insertBefore(e,a.firstChild);return e}
function Sv(a){a=Pv(a);var b=document.getElementById(a);b&&(Fq(a),b.parentNode.removeChild(b))}
function Tv(a,b){a&&b&&(a=""+Ra(b),(a=Qv[a])&&Dq(a))}
function Pv(a){var b=document.createElement("a");ec(b,a);a=b.href.replace(/^[a-zA-Z]+:\/\//,"//");return"js-"+jc(a)}
var Qv={};var Uv=[],Vv=!1;function Wv(){if(!M("disable_biscotti_fetch_for_ad_blocker_detection")&&!M("disable_biscotti_fetch_entirely_for_all_web_clients")&&Ts()){var a=L("PLAYER_VARS",{});if("1"!=sb(a)&&!Vn(a)){var b=function(){Vv=!0;"google_ad_status"in window?Kk("DCLKSTAT",1):Kk("DCLKSTAT",2)};
try{Nv("//static.doubleclick.net/instream/ad_status.js",b)}catch(c){}Uv.push(bi.R(function(){if(!(Vv||"google_ad_status"in window)){try{Tv("//static.doubleclick.net/instream/ad_status.js",b)}catch(c){}Vv=!0;Kk("DCLKSTAT",3)}},5E3))}}}
function Xv(){var a=Number(L("DCLKSTAT",0));return isNaN(a)?0:a}
;function Yv(){this.state=1;this.h=null}
l=Yv.prototype;
l.initialize=function(a,b,c){if(a.program){var d,e=null!=(d=a.interpreterScript)?d:null,f;d=null!=(f=a.interpreterUrl)?f:null;a.interpreterSafeScript&&(e=a.interpreterSafeScript,Cb("From proto message. b/166824318"),e=e.privateDoNotAccessOrElseSafeScriptWrappedValue||"",e=(f=zb())?f.createScript(e):e,e=(new Eb(e)).toString());a.interpreterSafeUrl&&(d=a.interpreterSafeUrl,Cb("From proto message. b/166824318"),d=Ib(d.privateDoNotAccessOrElseTrustedResourceUrlWrappedValue||"").toString());Zv(this,e,
d,a.program,b,c)}else ms(Error("Cannot initialize botguard without program"))};
function Zv(a,b,c,d,e,f){var g=void 0===g?"trayride":g;c?(a.state=2,Nv(c,function(){window[g]?$v(a,d,g,e):(a.state=3,Sv(c),ms(new P("Unable to load Botguard","from "+c)))},f)):b?(f=pf("SCRIPT"),f.textContent=b,f.nonce=hc(),document.head.appendChild(f),document.head.removeChild(f),window[g]?$v(a,d,g,e):(a.state=4,ms(new P("Unable to load Botguard from JS")))):ms(new P("Unable to load VM; no url or JS provided"))}
function $v(a,b,c,d){a.state=5;try{var e=new Hh({program:b,Tc:c,gd:M("att_web_record_metrics")});e.td.then(function(){a.state=6;d&&d(b)});
a.Ib(e)}catch(f){a.state=7,f instanceof Error&&ms(f)}}
l.invoke=function(a){a=void 0===a?{}:a;return this.Jb()?this.yc({Ub:a}):null};
l.dispose=function(){this.Kb()};
l.Kb=function(){this.Ib(null);this.state=8};
l.Jb=function(){return!!this.h};
l.yc=function(a){return this.h.tc(a)};
l.Ib=function(a){ue(this.h);this.h=a};function cw(){var a=B("yt.abuse.playerAttLoader");return a&&["bgvma","bgvmb","bgvmc"].every(function(b){return b in a})?a:null}
;function dw(){Yv.apply(this,arguments)}
u(dw,Yv);dw.prototype.Kb=function(){this.state=8};
dw.prototype.Ib=function(a){var b;null==(b=cw())||b.bgvma();a?(b={bgvma:a.dispose.bind(a),bgvmb:a.snapshot.bind(a),bgvmc:a.tc.bind(a)},A("yt.abuse.playerAttLoader",b),A("yt.abuse.playerAttLoaderRun",function(c){return a.snapshot(c)})):(A("yt.abuse.playerAttLoader",null),A("yt.abuse.playerAttLoaderRun",null))};
dw.prototype.Jb=function(){return!!cw()};
dw.prototype.yc=function(a){return cw().bgvmc(a)};var ew=new dw;function fw(){return ew.Jb()}
function gw(a){a=void 0===a?{}:a;a=void 0===a?{}:a;return ew.invoke(a)}
;function hw(a){var b=this;var c=void 0===c?0:c;var d=void 0===d?Yl():d;this.l=c;this.j=d;this.i=new Gh;this.h=a;a={};c=p(this.h.entries());for(d=c.next();!d.done;a={Fa:a.Fa,Sa:a.Sa},d=c.next()){var e=p(d.value);d=e.next().value;e=e.next().value;a.Sa=d;a.Fa=e;d=function(f){return function(){f.Fa.Ab();b.h[f.Sa].mb=!0;b.h.every(function(g){return!0===g.mb})&&b.i.resolve()}}(a);
e=Ul(d,iw(this,a.Fa));this.h[a.Sa]=Object.assign({},a.Fa,{Ab:d,jobId:e})}}
function jw(a){var b=Array.from(a.h.keys()).sort(function(d,e){return iw(a,a.h[e])-iw(a,a.h[d])});
b=p(b);for(var c=b.next();!c.done;c=b.next())c=a.h[c.value],void 0===c.jobId||c.mb||(a.j.ea(c.jobId),Ul(c.Ab,10))}
hw.prototype.cancel=function(){for(var a=p(this.h),b=a.next();!b.done;b=a.next())b=b.value,void 0===b.jobId||b.mb||this.j.ea(b.jobId),b.mb=!0;this.i.resolve()};
function iw(a,b){var c;return null!=(c=b.priority)?c:a.l}
;function kw(a){this.state=a;this.plugins=[];this.o=void 0}
kw.prototype.install=function(){this.plugins.push.apply(this.plugins,ia(Ja.apply(0,arguments)))};
kw.prototype.uninstall=function(){var a=this;Ja.apply(0,arguments).forEach(function(b){b=a.plugins.indexOf(b);-1<b&&a.plugins.splice(b,1)})};
kw.prototype.transition=function(a,b){var c=this,d=this.v.find(function(f){return Array.isArray(f.from)?f.from.find(function(g){return g===c.state&&f.D===a}):f.from===c.state&&f.D===a});
if(d){this.j&&(jw(this.j),this.j=void 0);this.state=a;d=d.action.bind(this);var e=this.plugins.filter(function(f){return f[a]}).map(function(f){return f[a]});
d(lw(this,e,this.o),b)}else throw Error("no transition specified from "+this.state+" to "+a);};
function lw(a,b,c){return function(){var d=Ja.apply(0,arguments),e=b.filter(function(k){var m;return 10===(null!=(m=null!=c?c:k.priority)?m:0)}),f=b.filter(function(k){var m;
return 10!==(null!=(m=null!=c?c:k.priority)?m:0)});
Yl();var g={};e=p(e);for(var h=e.next();!h.done;g={Ta:g.Ta},h=e.next())g.Ta=h.value,Wl(function(k){return function(){k.Ta.callback.apply(k.Ta,ia(d))}}(g));
f=f.map(function(k){var m;return{Ab:function(){k.callback.apply(k,ia(d))},
priority:null!=(m=null!=c?c:k.priority)?m:0}});
f.length&&(a.j=new hw(f))}}
ea.Object.defineProperties(kw.prototype,{currentState:{configurable:!0,enumerable:!0,get:function(){return this.state}}});function mw(a){kw.call(this,void 0===a?"document_active":a);var b=this;this.o=10;this.h=new Map;this.v=[{from:"document_active",D:"document_disposed_preventable",action:this.M},{from:"document_active",D:"document_disposed",action:this.l},{from:"document_disposed_preventable",D:"document_disposed",action:this.l},{from:"document_disposed_preventable",D:"flush_logs",action:this.m},{from:"document_disposed_preventable",D:"document_active",action:this.i},{from:"document_disposed",D:"flush_logs",action:this.m},
{from:"document_disposed",D:"document_active",action:this.i},{from:"document_disposed",D:"document_disposed",action:function(){}},
{from:"flush_logs",D:"document_active",action:this.i}];window.addEventListener("pagehide",function(c){b.transition("document_disposed",{event:c})});
window.addEventListener("beforeunload",function(c){b.transition("document_disposed_preventable",{event:c})})}
u(mw,kw);mw.prototype.M=function(a,b){if(!this.h.get("document_disposed_preventable")){a(null==b?void 0:b.event);var c,d;if((null==b?0:null==(c=b.event)?0:c.defaultPrevented)||(null==b?0:null==(d=b.event)?0:d.returnValue)){b.event.returnValue||(b.event.returnValue=!0);b.event.defaultPrevented||b.event.preventDefault();this.h=new Map;this.transition("document_active");return}}this.h.set("document_disposed_preventable",!0);this.h.get("document_disposed")?this.transition("flush_logs"):this.transition("document_disposed")};
mw.prototype.l=function(a,b){this.h.get("document_disposed")?this.transition("document_active"):(a(null==b?void 0:b.event),this.h.set("document_disposed",!0),this.transition("flush_logs"))};
mw.prototype.m=function(a,b){a(null==b?void 0:b.event);this.transition("document_active")};
mw.prototype.i=function(){this.h=new Map};function nw(a){kw.call(this,void 0===a?"document_visibility_unknown":a);var b=this;this.v=[{from:"document_visibility_unknown",D:"document_visible",action:this.i},{from:"document_visibility_unknown",D:"document_hidden",action:this.h},{from:"document_visibility_unknown",D:"document_foregrounded",action:this.m},{from:"document_visibility_unknown",D:"document_backgrounded",action:this.l},{from:"document_visible",D:"document_hidden",action:this.h},{from:"document_visible",D:"document_foregrounded",action:this.m},
{from:"document_visible",D:"document_visible",action:this.i},{from:"document_foregrounded",D:"document_visible",action:this.i},{from:"document_foregrounded",D:"document_hidden",action:this.h},{from:"document_foregrounded",D:"document_foregrounded",action:this.m},{from:"document_hidden",D:"document_visible",action:this.i},{from:"document_hidden",D:"document_backgrounded",action:this.l},{from:"document_hidden",D:"document_hidden",action:this.h},{from:"document_backgrounded",D:"document_hidden",action:this.h},
{from:"document_backgrounded",D:"document_backgrounded",action:this.l},{from:"document_backgrounded",D:"document_visible",action:this.i}];document.addEventListener("visibilitychange",function(c){"visible"===document.visibilityState?b.transition("document_visible",{event:c}):b.transition("document_hidden",{event:c})});
M("visibility_lifecycles_dynamic_backgrounding")&&(window.addEventListener("blur",function(c){b.transition("document_backgrounded",{event:c})}),window.addEventListener("focus",function(c){b.transition("document_foregrounded",{event:c})}))}
u(nw,kw);nw.prototype.i=function(a,b){a(null==b?void 0:b.event);M("visibility_lifecycles_dynamic_backgrounding")&&this.transition("document_foregrounded")};
nw.prototype.h=function(a,b){a(null==b?void 0:b.event);M("visibility_lifecycles_dynamic_backgrounding")&&this.transition("document_backgrounded")};
nw.prototype.l=function(a,b){a(null==b?void 0:b.event)};
nw.prototype.m=function(a,b){a(null==b?void 0:b.event)};function ow(){this.h=new mw;this.i=new nw}
ow.prototype.install=function(){var a=Ja.apply(0,arguments);this.h.install.apply(this.h,ia(a));this.i.install.apply(this.i,ia(a))};function pw(){ow.call(this);var a={};this.install((a.document_disposed={callback:this.j},a));a={};this.install((a.flush_logs={callback:this.l},a))}
var qw;u(pw,ow);pw.prototype.l=function(){if(M("web_fp_via_jspb")){var a=new Bj,b=Es();b&&D(a,1,b);b=new Yj;ce(b,Bj,380,Zj,a);Mr(b);M("web_fp_via_jspb_and_json")&&lm("finalPayload",{csn:Es()})}else lm("finalPayload",{csn:Es()})};
pw.prototype.j=function(){qs(rs)};function rw(){}
rw.getInstance=function(){var a=B("ytglobal.storage_");a||(a=new rw,A("ytglobal.storage_",a));return a};
rw.prototype.estimate=function(){var a,b,c;return x(function(d){a=navigator;return(null==(b=a.storage)?0:b.estimate)?d.return(a.storage.estimate()):(null==(c=a.webkitTemporaryStorage)?0:c.queryUsageAndQuota)?d.return(sw()):d.return()})};
function sw(){var a=navigator;return new Promise(function(b,c){var d;null!=(d=a.webkitTemporaryStorage)&&d.queryUsageAndQuota?a.webkitTemporaryStorage.queryUsageAndQuota(function(e,f){b({usage:e,quota:f})},function(e){c(e)}):c(Error("webkitTemporaryStorage is not supported."))})}
A("ytglobal.storageClass_",rw);function jm(a,b){var c=this;this.handleError=a;this.h=b;this.i=!1;void 0===self.document||self.addEventListener("beforeunload",function(){c.i=!0});
this.j=Math.random()<=Nk("ytidb_transaction_ended_event_rate_limit_session",.2)}
jm.prototype.logEvent=function(a,b){switch(a){case "IDB_DATA_CORRUPTED":M("idb_data_corrupted_killswitch")||this.h("idbDataCorrupted",b);break;case "IDB_UNEXPECTEDLY_CLOSED":this.h("idbUnexpectedlyClosed",b);break;case "IS_SUPPORTED_COMPLETED":M("idb_is_supported_completed_killswitch")||this.h("idbIsSupportedCompleted",b);break;case "QUOTA_EXCEEDED":tw(this,b);break;case "TRANSACTION_ENDED":this.j&&Math.random()<=Nk("ytidb_transaction_ended_event_rate_limit_transaction",.1)&&this.h("idbTransactionEnded",
b);break;case "TRANSACTION_UNEXPECTEDLY_ABORTED":a=Object.assign({},b,{hasWindowUnloaded:this.i}),this.h("idbTransactionAborted",a)}};
function tw(a,b){rw.getInstance().estimate().then(function(c){c=Object.assign({},b,{isSw:void 0===self.document,isIframe:self!==self.top,deviceStorageUsageMbytes:uw(null==c?void 0:c.usage),deviceStorageQuotaMbytes:uw(null==c?void 0:c.quota)});a.h("idbQuotaExceeded",c)})}
function uw(a){return"undefined"===typeof a?"-1":String(Math.ceil(a/1048576))}
;function vw(a,b,c){J.call(this);var d=this;c=c||L("POST_MESSAGE_ORIGIN")||window.document.location.protocol+"//"+window.document.location.hostname;this.l=b||null;this.targetOrigin="*";this.m=c;this.sessionId=null;this.i="widget";this.I=!!a;this.F=function(e){a:if(!("*"!=d.m&&e.origin!=d.m||d.l&&e.source!=d.l||"string"!==typeof e.data)){try{var f=JSON.parse(e.data)}catch(g){break a}if(!(null==f||d.I&&(d.sessionId&&d.sessionId!=f.id||d.i&&d.i!=f.channel))&&f)switch(f.event){case "listening":"null"!=
e.origin&&(d.m=d.targetOrigin=e.origin);d.l=e.source;d.sessionId=f.id;d.j&&(d.j(),d.j=null);break;case "command":d.o&&(!d.s||0<=eb(d.s,f.func))&&d.o(f.func,f.args,e.origin)}}};
this.s=this.j=this.o=null;window.addEventListener("message",this.F)}
u(vw,J);vw.prototype.sendMessage=function(a,b){if(b=b||this.l){this.sessionId&&(a.id=this.sessionId);this.i&&(a.channel=this.i);try{var c=JSON.stringify(a);b.postMessage(c,this.targetOrigin)}catch(d){Uk(d)}}};
vw.prototype.C=function(){window.removeEventListener("message",this.F);J.prototype.C.call(this)};function ww(){this.i=[];this.isReady=!1;this.j={};var a=this.h=new vw(!!L("WIDGET_ID_ENFORCE")),b=this.fd.bind(this);a.o=b;a.s=null;this.h.i="widget";if(a=L("WIDGET_ID"))this.h.sessionId=a}
l=ww.prototype;l.fd=function(a,b,c){"addEventListener"===a&&b?(a=b[0],this.j[a]||"onReady"===a||(this.addEventListener(a,xw(this,a)),this.j[a]=!0)):this.Lb(a,b,c)};
l.Lb=function(){};
function xw(a,b){return function(c){return a.sendMessage(b,c)}}
l.addEventListener=function(){};
l.Qc=function(){this.isReady=!0;this.sendMessage("initialDelivery",this.wb());this.sendMessage("onReady");fb(this.i,this.qc,this);this.i=[]};
l.wb=function(){return null};
function yw(a,b){a.sendMessage("infoDelivery",b)}
l.qc=function(a){this.isReady?this.h.sendMessage(a):this.i.push(a)};
l.sendMessage=function(a,b){this.qc({event:a,info:void 0===b?null:b})};
l.dispose=function(){this.h=null};var zw={},Aw=(zw["api.invalidparam"]=2,zw.auth=150,zw["drm.auth"]=150,zw["heartbeat.net"]=150,zw["heartbeat.servererror"]=150,zw["heartbeat.stop"]=150,zw["html5.unsupportedads"]=5,zw["fmt.noneavailable"]=5,zw["fmt.decode"]=5,zw["fmt.unplayable"]=5,zw["html5.missingapi"]=5,zw["html5.unsupportedlive"]=5,zw["drm.unavailable"]=5,zw["mrm.blocked"]=151,zw);var Bw=new Set("endSeconds startSeconds mediaContentUrl suggestedQuality videoId rct rctn".split(" "));function Cw(a){return(0===a.search("cue")||0===a.search("load"))&&"loadModule"!==a}
function Dw(a,b,c){if("string"===typeof a)return{videoId:a,startSeconds:b,suggestedQuality:c};b={};c=p(Bw);for(var d=c.next();!d.done;d=c.next())d=d.value,a[d]&&(b[d]=a[d]);return b}
function Ew(a,b,c,d){if(Qa(a)&&!Array.isArray(a)){b="playlist list listType index startSeconds suggestedQuality".split(" ");c={};for(d=0;d<b.length;d++){var e=b[d];a[e]&&(c[e]=a[e])}return c}b={index:b,startSeconds:c,suggestedQuality:d};"string"===typeof a&&16===a.length?b.list="PL"+a:b.playlist=a;return b}
;function Fw(a){ww.call(this);this.listeners=[];this.api=a;this.addEventListener("onReady",this.onReady.bind(this));this.addEventListener("onVideoProgress",this.qd.bind(this));this.addEventListener("onVolumeChange",this.rd.bind(this));this.addEventListener("onApiChange",this.ld.bind(this));this.addEventListener("onPlaybackQualityChange",this.nd.bind(this));this.addEventListener("onPlaybackRateChange",this.od.bind(this));this.addEventListener("onStateChange",this.pd.bind(this));this.addEventListener("onWebglSettingsChanged",
this.sd.bind(this))}
u(Fw,ww);l=Fw.prototype;
l.Lb=function(a,b,c){if(this.api.isExternalMethodAvailable(a,c)){b=b||[];if(0<b.length&&Cw(a)){var d=b;if(Qa(d[0])&&!Array.isArray(d[0]))var e=d[0];else switch(e={},a){case "loadVideoById":case "cueVideoById":e=Dw(d[0],void 0!==d[1]?Number(d[1]):void 0,d[2]);break;case "loadVideoByUrl":case "cueVideoByUrl":e=d[0];"string"===typeof e&&(e={mediaContentUrl:e,startSeconds:void 0!==d[1]?Number(d[1]):void 0,suggestedQuality:d[2]});b:{if((d=e.mediaContentUrl)&&(d=/\/([ve]|embed)\/([^#?]+)/.exec(d))&&d[2]){d=
d[2];break b}d=null}e.videoId=d;e=Dw(e);break;case "loadPlaylist":case "cuePlaylist":e=Ew(d[0],d[1],d[2],d[3])}b.length=1;b[0]=e}this.api.handleExternalCall(a,b,c);Cw(a)&&yw(this,this.wb())}};
l.onReady=function(){var a=this.Qc.bind(this);this.h.j=a;a=this.api.getVideoData();if(!a.isPlayable){a=a.errorCode;var b=void 0===b?5:b;this.sendMessage("onError",(a?Aw[a]||b:b).toString())}};
l.addEventListener=function(a,b){this.listeners.push({eventType:a,listener:b});this.api.addEventListener(a,b)};
l.wb=function(){if(!this.api)return null;var a=this.api.getApiInterface();kb(a,"getVideoData");for(var b={apiInterface:a},c=0,d=a.length;c<d;c++){var e=a[c];if(0===e.search("get")||0===e.search("is")){var f=0;0===e.search("get")?f=3:0===e.search("is")&&(f=2);f=e.charAt(f).toLowerCase()+e.substr(f+1);try{var g=this.api[e]();b[f]=g}catch(h){}}}b.videoData=this.api.getVideoData();b.currentTimeLastUpdated_=Date.now()/1E3;return b};
l.pd=function(a){a={playerState:a,currentTime:this.api.getCurrentTime(),duration:this.api.getDuration(),videoData:this.api.getVideoData(),videoStartBytes:0,videoBytesTotal:this.api.getVideoBytesTotal(),videoLoadedFraction:this.api.getVideoLoadedFraction(),playbackQuality:this.api.getPlaybackQuality(),availableQualityLevels:this.api.getAvailableQualityLevels(),currentTimeLastUpdated_:Date.now()/1E3,playbackRate:this.api.getPlaybackRate(),mediaReferenceTime:this.api.getMediaReferenceTime()};this.api.getVideoUrl&&
(a.videoUrl=this.api.getVideoUrl());this.api.getVideoContentRect&&(a.videoContentRect=this.api.getVideoContentRect());this.api.getProgressState&&(a.progressState=this.api.getProgressState());this.api.getPlaylist&&(a.playlist=this.api.getPlaylist());this.api.getPlaylistIndex&&(a.playlistIndex=this.api.getPlaylistIndex());this.api.getStoryboardFormat&&(a.storyboardFormat=this.api.getStoryboardFormat());yw(this,a)};
l.nd=function(a){yw(this,{playbackQuality:a})};
l.od=function(a){yw(this,{playbackRate:a})};
l.ld=function(){for(var a=this.api.getOptions(),b={namespaces:a},c=0,d=a.length;c<d;c++){var e=a[c],f=this.api.getOptions(e);b[e]={options:f};for(var g=0,h=f.length;g<h;g++){var k=f[g],m=this.api.getOption(e,k);b[e][k]=m}}this.sendMessage("apiInfoDelivery",b)};
l.rd=function(){yw(this,{muted:this.api.isMuted(),volume:this.api.getVolume()})};
l.qd=function(a){a={currentTime:a,videoBytesLoaded:this.api.getVideoBytesLoaded(),videoLoadedFraction:this.api.getVideoLoadedFraction(),currentTimeLastUpdated_:Date.now()/1E3,playbackRate:this.api.getPlaybackRate(),mediaReferenceTime:this.api.getMediaReferenceTime()};this.api.getProgressState&&(a.progressState=this.api.getProgressState());yw(this,a)};
l.sd=function(){var a={sphericalProperties:this.api.getSphericalProperties()};yw(this,a)};
l.dispose=function(){ww.prototype.dispose.call(this);for(var a=0;a<this.listeners.length;a++){var b=this.listeners[a];this.api.removeEventListener(b.eventType,b.listener)}this.listeners=[]};function Gw(a){J.call(this);this.i={};this.started=!1;this.connection=a;this.connection.subscribe("command",this.lc,this)}
u(Gw,J);l=Gw.prototype;l.start=function(){this.started||this.h()||(this.started=!0,this.connection.ya("RECEIVING"))};
l.ya=function(a,b){this.started&&!this.h()&&this.connection.ya(a,b)};
l.lc=function(a,b,c){if(this.started&&!this.h()){var d=b||{};switch(a){case "addEventListener":"string"===typeof d.event&&this.addListener(d.event);break;case "removeEventListener":"string"===typeof d.event&&this.removeListener(d.event);break;default:this.api.isReady()&&this.api.isExternalMethodAvailable(a,c||null)&&(b=Hw(a,b||{}),c=this.api.handleExternalCall(a,b,c||null),(c=Iw(a,c))&&this.ya(a,c))}}};
l.addListener=function(a){if(!(a in this.i)){var b=this.md.bind(this,a);this.i[a]=b;this.addEventListener(a,b)}};
l.md=function(a,b){this.started&&!this.h()&&this.connection.ya(a,this.vb(a,b))};
l.vb=function(a,b){if(null!=b)return{value:b}};
l.removeListener=function(a){a in this.i&&(this.removeEventListener(a,this.i[a]),delete this.i[a])};
l.C=function(){var a=this.connection;a.h()||yi(a.i,"command",this.lc,this);this.connection=null;for(var b in this.i)this.i.hasOwnProperty(b)&&this.removeListener(b);J.prototype.C.call(this)};function Jw(a,b){Gw.call(this,b);this.api=a;this.start()}
u(Jw,Gw);Jw.prototype.addEventListener=function(a,b){this.api.addEventListener(a,b)};
Jw.prototype.removeEventListener=function(a,b){this.api.removeEventListener(a,b)};
function Hw(a,b){switch(a){case "loadVideoById":return a=Dw(b),[a];case "cueVideoById":return a=Dw(b),[a];case "loadVideoByPlayerVars":return[b];case "cueVideoByPlayerVars":return[b];case "loadPlaylist":return a=Ew(b),[a];case "cuePlaylist":return a=Ew(b),[a];case "seekTo":return[b.seconds,b.allowSeekAhead];case "playVideoAt":return[b.index];case "setVolume":return[b.volume];case "setPlaybackQuality":return[b.suggestedQuality];case "setPlaybackRate":return[b.suggestedRate];case "setLoop":return[b.loopPlaylists];
case "setShuffle":return[b.shufflePlaylist];case "getOptions":return[b.module];case "getOption":return[b.module,b.option];case "setOption":return[b.module,b.option,b.value];case "handleGlobalKeyDown":return[b.keyCode,b.shiftKey,b.ctrlKey,b.altKey,b.metaKey,b.key,b.code]}return[]}
function Iw(a,b){switch(a){case "isMuted":return{muted:b};case "getVolume":return{volume:b};case "getPlaybackRate":return{playbackRate:b};case "getAvailablePlaybackRates":return{availablePlaybackRates:b};case "getVideoLoadedFraction":return{videoLoadedFraction:b};case "getPlayerState":return{playerState:b};case "getCurrentTime":return{currentTime:b};case "getPlaybackQuality":return{playbackQuality:b};case "getAvailableQualityLevels":return{availableQualityLevels:b};case "getDuration":return{duration:b};
case "getVideoUrl":return{videoUrl:b};case "getVideoEmbedCode":return{videoEmbedCode:b};case "getPlaylist":return{playlist:b};case "getPlaylistIndex":return{playlistIndex:b};case "getOptions":return{options:b};case "getOption":return{option:b}}}
Jw.prototype.vb=function(a,b){switch(a){case "onReady":return;case "onStateChange":return{playerState:b};case "onPlaybackQualityChange":return{playbackQuality:b};case "onPlaybackRateChange":return{playbackRate:b};case "onError":return{errorCode:b}}return Gw.prototype.vb.call(this,a,b)};
Jw.prototype.C=function(){Gw.prototype.C.call(this);delete this.api};function Kw(a){a=void 0===a?!1:a;J.call(this);this.i=new K(a);we(this,this.i)}
Za(Kw,J);Kw.prototype.subscribe=function(a,b,c){return this.h()?0:this.i.subscribe(a,b,c)};
Kw.prototype.m=function(a,b){this.h()||this.i.ra.apply(this.i,arguments)};function Lw(a,b,c){Kw.call(this);this.l=a;this.j=b;this.id=c}
u(Lw,Kw);Lw.prototype.ya=function(a,b){this.h()||this.l.ya(this.j,this.id,a,b)};
Lw.prototype.C=function(){this.j=this.l=null;Kw.prototype.C.call(this)};function Mw(a,b,c){J.call(this);this.i=a;this.origin=c;this.j=oq(window,"message",this.l.bind(this));this.connection=new Lw(this,a,b);we(this,this.connection)}
u(Mw,J);Mw.prototype.ya=function(a,b,c,d){this.h()||a!==this.i||(a={id:b,command:c},d&&(a.data=d),this.i.postMessage(JSON.stringify(a),this.origin))};
Mw.prototype.l=function(a){if(!this.h()&&a.origin===this.origin){var b=a.data;if("string"===typeof b){try{b=JSON.parse(b)}catch(d){return}if(b.command){var c=this.connection;c.h()||c.m("command",b.command,b.data,a.origin)}}}};
Mw.prototype.C=function(){pq(this.j);this.i=null;J.prototype.C.call(this)};function Nw(a){a=a||{};var b={},c={};this.url=a.url||"";this.args=a.args||ub(b);this.assets=a.assets||{};this.attrs=a.attrs||ub(c);this.fallback=a.fallback||null;this.fallbackMessage=a.fallbackMessage||null;this.html5=!!a.html5;this.disable=a.disable||{};this.loaded=!!a.loaded;this.messages=a.messages||{}}
Nw.prototype.clone=function(){var a=new Nw,b;for(b in this)if(this.hasOwnProperty(b)){var c=this[b];"object"==Oa(c)?a[b]=ub(c):a[b]=c}return a};var Ow=/cssbin\/(?:debug-)?([a-zA-Z0-9_-]+?)(?:-2x|-web|-rtl|-vfl|.css)/;function Pw(a){a=a||"";if(window.spf){var b=a.match(Ow);spf.style.load(a,b?b[1]:"",void 0)}else Qw(a)}
function Qw(a){var b=Rw(a),c=document.getElementById(b),d=c&&Jv(c,"loaded");d||c&&!d||(c=Sw(a,b,function(){Jv(c,"loaded")||(Hv(c),Eq(b),ml(Ya(Fq,b),0))}))}
function Sw(a,b,c){var d=document.createElement("link");d.id=b;d.onload=function(){c&&setTimeout(c,0)};
a=Ib(a);fc(d,a);(document.getElementsByTagName("head")[0]||document.body).appendChild(d);return d}
function Rw(a){var b=pf("A");Cb("This URL is never added to the DOM");ec(b,new Nb(a,Ob));a=b.href.replace(/^[a-zA-Z]+:\/\//,"//");return"css-"+jc(a)}
;function Tw(){J.call(this);this.i=[]}
u(Tw,J);Tw.prototype.C=function(){for(;this.i.length;){var a=this.i.pop();a.target.removeEventListener(a.name,a.callback,void 0)}J.prototype.C.call(this)};function Uw(){Tw.apply(this,arguments)}
u(Uw,Tw);function Vw(a,b,c,d,e){J.call(this);var f=this;this.s=b;this.webPlayerContextConfig=d;this.Xa=e;this.ca=!1;this.api={};this.V=this.o=null;this.K=new K;this.i={};this.P=this.W=this.elementId=this.sa=this.config=null;this.O=!1;this.l=this.F=null;this.ja={};this.Ya=["onReady"];this.lastError=null;this.Ha=NaN;this.I={};this.Za=new Uw(this);this.S=0;this.j=this.m=a;we(this,this.K);Ww(this);Xw(this);we(this,this.Za);c?this.S=ml(function(){f.loadNewVideoConfig(c)},0):d&&(Yw(this),Zw(this))}
u(Vw,J);l=Vw.prototype;l.getId=function(){return this.s};
l.loadNewVideoConfig=function(a){if(!this.h()){this.S&&(window.clearTimeout(this.S),this.S=0);var b=a||{};b instanceof Nw||(b=new Nw(b));this.config=b;this.setConfig(a);Zw(this);this.isReady()&&$w(this)}};
function Yw(a){var b;a.webPlayerContextConfig?b=a.webPlayerContextConfig.rootElementId:b=a.config.attrs.id;a.elementId=b||a.elementId;"video-player"===a.elementId&&(a.elementId=a.s,a.webPlayerContextConfig?a.webPlayerContextConfig.rootElementId=a.s:a.config.attrs.id=a.s);var c;(null==(c=a.j)?void 0:c.id)===a.elementId&&(a.elementId+="-player",a.webPlayerContextConfig?a.webPlayerContextConfig.rootElementId=a.elementId:a.config.attrs.id=a.elementId)}
l.setConfig=function(a){this.sa=a;this.config=ax(a);Yw(this);if(!this.W){var b;this.W=bx(this,(null==(b=this.config.args)?void 0:b.jsapicallback)||"onYouTubePlayerReady")}this.config.args?this.config.args.jsapicallback=null:this.config.args={jsapicallback:null};var c;if(null==(c=this.config)?0:c.attrs)a=this.config.attrs,(b=a.width)&&this.j&&(this.j.style.width=Vh(Number(b)||b)),(a=a.height)&&this.j&&(this.j.style.height=Vh(Number(a)||a))};
function $w(a){if(a.config&&!0!==a.config.loaded)if(a.config.loaded=!0,!a.config.args||"0"!==a.config.args.autoplay&&0!==a.config.args.autoplay&&!1!==a.config.args.autoplay){var b;a.api.loadVideoByPlayerVars(null!=(b=a.config.args)?b:null)}else a.api.cueVideoByPlayerVars(a.config.args)}
function cx(a){var b=!0,c=dx(a);c&&a.config&&(a=ex(a),b=Jv(c,"version")===a);return b&&!!B("yt.player.Application.create")}
function Zw(a){if(!a.h()&&!a.O){var b=cx(a);if(b&&"html5"===(dx(a)?"html5":null))a.P="html5",a.isReady()||fx(a);else if(gx(a),a.P="html5",b&&a.l&&a.m)a.m.appendChild(a.l),fx(a);else{a.config&&(a.config.loaded=!0);var c=!1;a.F=function(){c=!0;var d=hx(a,"player_bootstrap_method")?B("yt.player.Application.createAlternate")||B("yt.player.Application.create"):B("yt.player.Application.create");var e=a.config?ax(a.config):void 0;d&&d(a.m,e,a.webPlayerContextConfig,a.Xa);fx(a)};
a.O=!0;b?a.F():(Nv(ex(a),a.F),(b=ix(a))&&Pw(b),jx(a)&&!c&&A("yt.player.Application.create",null))}}}
function dx(a){var b=of(a.elementId);!b&&a.j&&a.j.querySelector&&(b=a.j.querySelector("#"+a.elementId));return b}
function fx(a){if(!a.h()){var b=dx(a),c=!1;b&&b.getApiInterface&&b.getApiInterface()&&(c=!0);if(c){a.O=!1;if(!hx(a,"html5_remove_not_servable_check_killswitch")){var d;if((null==b?0:b.isNotServable)&&a.config&&(null==b?0:b.isNotServable(null==(d=a.config.args)?void 0:d.video_id)))return}kx(a)}else a.Ha=ml(function(){fx(a)},50)}}
function kx(a){Ww(a);a.ca=!0;var b=dx(a);if(b){a.o=lx(a,b,"addEventListener");a.V=lx(a,b,"removeEventListener");var c=b.getApiInterface();c=c.concat(b.getInternalApiInterface());for(var d=a.api,e=0;e<c.length;e++){var f=c[e];d[f]||(d[f]=lx(a,b,f))}}for(var g in a.i)a.i.hasOwnProperty(g)&&a.o&&a.o(g,a.i[g]);$w(a);a.W&&a.W(a.api);a.K.ra("onReady",a.api)}
function lx(a,b,c){var d=b[c];return function(){var e=Ja.apply(0,arguments);try{return a.lastError=null,d.apply(b,e)}catch(f){"sendAbandonmentPing"!==c&&(f.params=c,a.lastError=f,ms(f))}}}
function Ww(a){a.ca=!1;if(a.V)for(var b in a.i)a.i.hasOwnProperty(b)&&a.V(b,a.i[b]);for(var c in a.I)a.I.hasOwnProperty(c)&&window.clearTimeout(Number(c));a.I={};a.o=null;a.V=null;b=a.api;for(var d in b)b.hasOwnProperty(d)&&(b[d]=null);b.addEventListener=function(e,f){a.addEventListener(e,f)};
b.removeEventListener=function(e,f){a.removeEventListener(e,f)};
b.destroy=function(){a.dispose()};
b.getLastError=function(){return a.getLastError()};
b.getPlayerType=function(){return a.getPlayerType()};
b.getCurrentVideoConfig=function(){return a.sa};
b.loadNewVideoConfig=function(e){a.loadNewVideoConfig(e)};
b.isReady=function(){return a.isReady()}}
l.isReady=function(){return this.ca};
function Xw(a){a.addEventListener("WATCH_LATER_VIDEO_ADDED",function(b){Eq("WATCH_LATER_VIDEO_ADDED",b)});
a.addEventListener("WATCH_LATER_VIDEO_REMOVED",function(b){Eq("WATCH_LATER_VIDEO_REMOVED",b)})}
l.addEventListener=function(a,b){var c=this,d=bx(this,b);d&&(0<=eb(this.Ya,a)||this.i[a]||(b=mx(this,a),this.o&&this.o(a,b)),this.K.subscribe(a,d),"onReady"===a&&this.isReady()&&ml(function(){d(c.api)},0))};
l.removeEventListener=function(a,b){this.h()||(b=bx(this,b))&&yi(this.K,a,b)};
function bx(a,b){var c=b;if("string"===typeof b){if(a.ja[b])return a.ja[b];c=function(){var d=Ja.apply(0,arguments),e=B(b);if(e)try{e.apply(z,d)}catch(f){ls(f)}};
a.ja[b]=c}return c?c:null}
function mx(a,b){var c="ytPlayer"+b+a.s;a.i[b]=c;z[c]=function(d){var e=ml(function(){if(!a.h()){try{a.K.ra(b,null!=d?d:void 0)}catch(h){ms(new P("PlayerProxy error when creating global callback",{error:h,event:b,playerId:a.s,data:d}))}var f=a.I,g=String(e);g in f&&delete f[g]}},0);
rb(a.I,String(e))};
return c}
l.getPlayerType=function(){return this.P||(dx(this)?"html5":null)};
l.getLastError=function(){return this.lastError};
function gx(a){a.cancel();Ww(a);a.P=null;a.config&&(a.config.loaded=!1);var b=dx(a);b&&(cx(a)||!jx(a)?a.l=b:(b&&b.destroy&&b.destroy(),a.l=null));if(a.m)for(a=a.m;b=a.firstChild;)a.removeChild(b)}
l.cancel=function(){this.F&&Tv(ex(this),this.F);window.clearTimeout(this.Ha);this.O=!1};
l.C=function(){gx(this);if(this.l&&this.config&&this.l.destroy)try{this.l.destroy()}catch(b){ls(b)}this.ja=null;for(var a in this.i)this.i.hasOwnProperty(a)&&(z[this.i[a]]=null);this.sa=this.config=this.api=null;delete this.m;delete this.j;J.prototype.C.call(this)};
function jx(a){var b,c;a=null==(b=a.config)?void 0:null==(c=b.args)?void 0:c.fflags;return!!a&&-1!==a.indexOf("player_destroy_old_version=true")}
function ex(a){return a.webPlayerContextConfig?a.webPlayerContextConfig.jsUrl:(a=a.config.assets)?a.js:""}
function ix(a){return a.webPlayerContextConfig?a.webPlayerContextConfig.cssUrl:(a=a.config.assets)?a.css:""}
function hx(a,b){if(a.webPlayerContextConfig)var c=a.webPlayerContextConfig.serializedExperimentFlags;else{var d;if(null==(d=a.config)?0:d.args)c=a.config.args.fflags}return"true"===$k(c||"","&")[b]}
function ax(a){for(var b={},c=p(Object.keys(a)),d=c.next();!d.done;d=c.next()){d=d.value;var e=a[d];b[d]="object"===typeof e?ub(e):e}return b}
;var nx={},ox="player_uid_"+(1E9*Math.random()>>>0);function px(a,b){var c="player",d=!1;d=void 0===d?!0:d;c="string"===typeof c?of(c):c;var e=ox+"_"+Ra(c),f=nx[e];if(f&&d)return qx(a,b)?f.api.loadVideoByPlayerVars(a.args||null):f.loadNewVideoConfig(a),f.api;f=new Vw(c,e,a,b,void 0);nx[e]=f;Eq("player-added",f.api);xe(f,function(){delete nx[f.getId()]});
return f.api}
function qx(a,b){return b&&b.serializedExperimentFlags?b.serializedExperimentFlags.includes("web_player_remove_playerproxy=true"):a&&a.args&&a.args.fflags?a.args.fflags.includes("web_player_remove_playerproxy=true"):!1}
;var rx=null,sx=null,tx=null;function ux(){vx()}
function wx(){vx()}
function vx(){var a=rx.getVideoData(1);a=a.title?a.title+" - YouTube":"YouTube";document.title!==a&&(document.title=a)}
function xx(){rx&&rx.sendAbandonmentPing&&rx.sendAbandonmentPing();L("PL_ATT")&&ew.dispose();for(var a=bi,b=0,c=Uv.length;b<c;b++)a.ea(Uv[b]);Uv.length=0;Sv("//static.doubleclick.net/instream/ad_status.js");Vv=!1;Kk("DCLKSTAT",0);ve(tx,sx);rx&&(rx.removeEventListener("onVideoDataChange",ux),rx.destroy())}
;function yx(a,b,c){a="ST-"+jc(a).toString(36);b=b?sc(b):"";c=c||5;Ts()&&Cl(a,b,c)}
;function zx(a,b,c){b=void 0===b?{}:b;c=void 0===c?!1:c;var d=L("EVENT_ID");d&&(b.ei||(b.ei=d));if(b){d=a;var e=void 0===e?!0:e;var f=L("VALID_SESSION_TEMPDATA_DOMAINS",[]),g=nc(window.location.href);g&&f.push(g);g=nc(d);if(0<=eb(f,g)||!g&&0==d.lastIndexOf("/",0))if(M("autoescape_tempdata_url")&&(f=document.createElement("a"),ec(f,d),d=f.href),d&&(d=oc(d),f=d.indexOf("#"),d=0>f?d:d.slice(0,f)))if(e&&!b.csn&&(b.itct||b.ved)&&(b=Object.assign({csn:Es()},b)),h){var h=parseInt(h,10);isFinite(h)&&0<h&&
yx(d,b,h)}else yx(d,b)}if(c)return!1;if((window.ytspf||{}).enabled)spf.navigate(a);else{var k=void 0===k?{}:k;var m=void 0===m?"":m;var q=void 0===q?window:q;c=q.location;a=uc(a,k)+m;var r=void 0===r?Oh:r;a:{r=void 0===r?Oh:r;for(k=0;k<r.length;++k)if(m=r[k],m instanceof Mh&&m.Yc(a)){r=new Nb(a,Ob);break a}r=void 0}r=r||Rb;if(r instanceof Nb)var w=Pb(r);else{b:if(Kh){try{w=new URL(r)}catch(t){w="https:";break b}w=w.protocol}else c:{w=document.createElement("a");try{w.href=r}catch(t){w=void 0;break c}w=
w.protocol;w=":"===w||""===w?"https:":w}w="javascript:"!==w?r:void 0}void 0!==w&&(c.href=w)}return!0}
;A("yt.setConfig",Kk);A("yt.config.set",Kk);A("yt.setMsg",Js);A("yt.msgs.set",Js);A("yt.logging.errors.log",ls);
A("writeEmbed",function(){var a=L("PLAYER_CONFIG");if(!a){var b=L("PLAYER_VARS");b&&(a={args:b})}bt(!0);"gvn"===a.args.ps&&(document.body.style.backgroundColor="transparent");a.attrs||(a.attrs={width:"100%",height:"100%",id:"video-player"});var c=document.referrer;b=L("POST_MESSAGE_ORIGIN");window!==window.top&&c&&c!==document.URL&&(a.args.loaderUrl=c);uu();c=L("WEB_PLAYER_CONTEXT_CONFIGS").WEB_PLAYER_CONTEXT_CONFIG_ID_EMBEDDED_PLAYER;if(!c.serializedForcedExperimentIds){var d=el(window.location.href);
d.forced_experiments&&(c.serializedForcedExperimentIds=d.forced_experiments)}rx=px(a,c);rx.addEventListener("onVideoDataChange",ux);rx.addEventListener("onReady",wx);a=L("POST_MESSAGE_ID","player");L("ENABLE_JS_API")?tx=new Fw(rx):L("ENABLE_POST_API")&&"string"===typeof a&&"string"===typeof b&&(sx=new Mw(window.parent,a,b),tx=new Jw(rx,sx.connection));Wv();M("ytidb_create_logger_embed_killswitch")||M("embeds_web_disable_nwl")||im();a={};qw||(qw=new pw);qw.install((a.flush_logs={callback:function(){or()}},
a));
M("embeds_web_disable_nwl")||eq();M("ytidb_clear_embedded_player")&&bi.R(function(){var e;if(!cv){var f=Pq(),g={Fb:bv,wc:av};f.h.set(g.Fb,g);g={Tb:{feedbackEndpoint:pt(Xu),modifyChannelNotificationPreferenceEndpoint:pt(Yu),playlistEditEndpoint:pt(Zu),subscribeEndpoint:pt(Vu),unsubscribeEndpoint:pt(Wu),webPlayerShareEntityServiceEndpoint:pt($u)}};var h=lt.getInstance(),k={};h&&(k.client_location=h);if(void 0===m){ct.h||(ct.h=new ct);var m=ct.h}void 0===e&&(e=f.resolve(bv));Iu(g,e,m,k);m={Fb:Ou,xc:Hu.h};
f.h.set(m.Fb,m);cv=f.resolve(Ou)}Gv()})});
var Ax=Sk(function(){Bu();var a=Gl.getInstance(),b=Jl(119),c=1<window.devicePixelRatio;if(document.body&&li(document.body,"exp-invert-logo"))if(c&&!li(document.body,"inverted-hdpi")){var d=document.body;if(d.classList)d.classList.add("inverted-hdpi");else if(!li(d,"inverted-hdpi")){var e=ji(d);ki(d,e+(0<e.length?" inverted-hdpi":"inverted-hdpi"))}}else!c&&li(document.body,"inverted-hdpi")&&mi();if(b!=c){b="f"+(Math.floor(119/31)+1);d=Kl(b)||0;d=c?d|67108864:d&-67108865;0==d?delete Fl[b]:(c=d.toString(16),
Fl[b]=c.toString());c=!0;M("web_secure_pref_cookie_killswitch")&&(c=!1);b=a.h;d=[];for(var f in Fl)d.push(f+"="+encodeURIComponent(String(Fl[f])));Cl(b,d.join("&"),63072E3,a.i,c)}ov.h||(ov.h=new ov);a=ov.h;f=16623;var g=void 0===g?{}:g;Object.values(Ks).includes(f)||(ms(new P("createClientScreen() called with a non-page VE",f)),f=83769);g.isHistoryNavigation||a.h.push({rootVe:f,key:g.key||""});a.o=[];a.v=[];g.Wb?sv(a,f,g):tv(a,f,g)}),Bx=Sk(function(a){M("embeds_web_enable_dispose_player_if_page_not_cached_killswitch")?
xx():a.persisted||(Yc?setTimeout(function(){xx()},0):xx())}),Cx=Sk(xx);
window.addEventListener?(window.addEventListener("load",Ax),Yc?window.addEventListener("beforeunload",Bx):window.addEventListener("pagehide",Bx)):window.attachEvent&&(window.attachEvent("onload",Ax),window.attachEvent("onunload",Cx));A("yt.abuse.player.botguardInitialized",B("yt.abuse.player.botguardInitialized")||fw);A("yt.abuse.player.invokeBotguard",B("yt.abuse.player.invokeBotguard")||gw);A("yt.abuse.dclkstatus.checkDclkStatus",B("yt.abuse.dclkstatus.checkDclkStatus")||Xv);
A("yt.player.exports.navigate",B("yt.player.exports.navigate")||zx);A("yt.util.activity.init",B("yt.util.activity.init")||tq);A("yt.util.activity.getTimeSinceActive",B("yt.util.activity.getTimeSinceActive")||wq);A("yt.util.activity.setTimestamp",B("yt.util.activity.setTimestamp")||uq);}).call(this);
