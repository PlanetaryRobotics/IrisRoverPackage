(function(){/*

 Copyright The Closure Library Authors.
 SPDX-License-Identifier: Apache-2.0
*/
'use strict';var l;function aa(a){var b=0;return function(){return b<a.length?{done:!1,value:a[b++]}:{done:!0}}}
var ba="function"==typeof Object.defineProperties?Object.defineProperty:function(a,b,c){if(a==Array.prototype||a==Object.prototype)return a;a[b]=c.value;return a};
function ca(a){a=["object"==typeof globalThis&&globalThis,a,"object"==typeof window&&window,"object"==typeof self&&self,"object"==typeof global&&global];for(var b=0;b<a.length;++b){var c=a[b];if(c&&c.Math==Math)return c}throw Error("Cannot find global object");}
var da=ca(this);function n(a,b){if(b)a:{var c=da;a=a.split(".");for(var d=0;d<a.length-1;d++){var e=a[d];if(!(e in c))break a;c=c[e]}a=a[a.length-1];d=c[a];b=b(d);b!=d&&null!=b&&ba(c,a,{configurable:!0,writable:!0,value:b})}}
n("Symbol",function(a){function b(f){if(this instanceof b)throw new TypeError("Symbol is not a constructor");return new c(d+(f||"")+"_"+e++,f)}
function c(f,g){this.h=f;ba(this,"description",{configurable:!0,writable:!0,value:g})}
if(a)return a;c.prototype.toString=function(){return this.h};
var d="jscomp_symbol_"+(1E9*Math.random()>>>0)+"_",e=0;return b});
n("Symbol.iterator",function(a){if(a)return a;a=Symbol("Symbol.iterator");for(var b="Array Int8Array Uint8Array Uint8ClampedArray Int16Array Uint16Array Int32Array Uint32Array Float32Array Float64Array".split(" "),c=0;c<b.length;c++){var d=da[b[c]];"function"===typeof d&&"function"!=typeof d.prototype[a]&&ba(d.prototype,a,{configurable:!0,writable:!0,value:function(){return ea(aa(this))}})}return a});
function ea(a){a={next:a};a[Symbol.iterator]=function(){return this};
return a}
function p(a){var b="undefined"!=typeof Symbol&&Symbol.iterator&&a[Symbol.iterator];return b?b.call(a):{next:aa(a)}}
function fa(a){if(!(a instanceof Array)){a=p(a);for(var b,c=[];!(b=a.next()).done;)c.push(b.value);a=c}return a}
function ha(a,b){return Object.prototype.hasOwnProperty.call(a,b)}
var ja="function"==typeof Object.assign?Object.assign:function(a,b){for(var c=1;c<arguments.length;c++){var d=arguments[c];if(d)for(var e in d)ha(d,e)&&(a[e]=d[e])}return a};
n("Object.assign",function(a){return a||ja});
var ka="function"==typeof Object.create?Object.create:function(a){function b(){}
b.prototype=a;return new b},la=function(){function a(){function c(){}
new c;Reflect.construct(c,[],function(){});
return new c instanceof c}
if("undefined"!=typeof Reflect&&Reflect.construct){if(a())return Reflect.construct;var b=Reflect.construct;return function(c,d,e){c=b(c,d);e&&Reflect.setPrototypeOf(c,e.prototype);return c}}return function(c,d,e){void 0===e&&(e=c);
e=ka(e.prototype||Object.prototype);return Function.prototype.apply.call(c,e,d)||e}}(),ma;
if("function"==typeof Object.setPrototypeOf)ma=Object.setPrototypeOf;else{var na;a:{var oa={a:!0},pa={};try{pa.__proto__=oa;na=pa.a;break a}catch(a){}na=!1}ma=na?function(a,b){a.__proto__=b;if(a.__proto__!==b)throw new TypeError(a+" is not extensible");return a}:null}var qa=ma;
function r(a,b){a.prototype=ka(b.prototype);a.prototype.constructor=a;if(qa)qa(a,b);else for(var c in b)if("prototype"!=c)if(Object.defineProperties){var d=Object.getOwnPropertyDescriptor(b,c);d&&Object.defineProperty(a,c,d)}else a[c]=b[c];a.aa=b.prototype}
function ra(){this.C=!1;this.l=null;this.i=void 0;this.h=1;this.o=this.m=0;this.A=this.j=null}
function ua(a){if(a.C)throw new TypeError("Generator is already running");a.C=!0}
ra.prototype.u=function(a){this.i=a};
function va(a,b){a.j={ub:b,yb:!0};a.h=a.m||a.o}
ra.prototype.return=function(a){this.j={return:a};this.h=this.o};
function t(a,b,c){a.h=c;return{value:b}}
ra.prototype.s=function(a){this.h=a};
function wa(a,b,c){a.m=b;void 0!=c&&(a.o=c)}
function xa(a,b){a.h=b;a.m=0}
function ya(a){a.m=0;var b=a.j.ub;a.j=null;return b}
function za(a){a.A=[a.j];a.m=0;a.o=0}
function Ca(a){var b=a.A.splice(0)[0];(b=a.j=a.j||b)?b.yb?a.h=a.m||a.o:void 0!=b.s&&a.o<b.s?(a.h=b.s,a.j=null):a.h=a.o:a.h=0}
function Da(a){this.h=new ra;this.i=a}
function Ea(a,b){ua(a.h);var c=a.h.l;if(c)return Fa(a,"return"in c?c["return"]:function(d){return{value:d,done:!0}},b,a.h.return);
a.h.return(b);return Ga(a)}
function Fa(a,b,c,d){try{var e=b.call(a.h.l,c);if(!(e instanceof Object))throw new TypeError("Iterator result "+e+" is not an object");if(!e.done)return a.h.C=!1,e;var f=e.value}catch(g){return a.h.l=null,va(a.h,g),Ga(a)}a.h.l=null;d.call(a.h,f);return Ga(a)}
function Ga(a){for(;a.h.h;)try{var b=a.i(a.h);if(b)return a.h.C=!1,{value:b.value,done:!1}}catch(c){a.h.i=void 0,va(a.h,c)}a.h.C=!1;if(a.h.j){b=a.h.j;a.h.j=null;if(b.yb)throw b.ub;return{value:b.return,done:!0}}return{value:void 0,done:!0}}
function Ha(a){this.next=function(b){ua(a.h);a.h.l?b=Fa(a,a.h.l.next,b,a.h.u):(a.h.u(b),b=Ga(a));return b};
this.throw=function(b){ua(a.h);a.h.l?b=Fa(a,a.h.l["throw"],b,a.h.u):(va(a.h,b),b=Ga(a));return b};
this.return=function(b){return Ea(a,b)};
this[Symbol.iterator]=function(){return this}}
function Ia(a){function b(d){return a.next(d)}
function c(d){return a.throw(d)}
return new Promise(function(d,e){function f(g){g.done?d(g.value):Promise.resolve(g.value).then(b,c).then(f,e)}
f(a.next())})}
function w(a){return Ia(new Ha(new Da(a)))}
function Ja(){for(var a=Number(this),b=[],c=a;c<arguments.length;c++)b[c-a]=arguments[c];return b}
n("Reflect",function(a){return a?a:{}});
n("Reflect.construct",function(){return la});
n("Reflect.setPrototypeOf",function(a){return a?a:qa?function(b,c){try{return qa(b,c),!0}catch(d){return!1}}:null});
n("Promise",function(a){function b(g){this.h=0;this.j=void 0;this.i=[];this.C=!1;var h=this.l();try{g(h.resolve,h.reject)}catch(k){h.reject(k)}}
function c(){this.h=null}
function d(g){return g instanceof b?g:new b(function(h){h(g)})}
if(a)return a;c.prototype.i=function(g){if(null==this.h){this.h=[];var h=this;this.j(function(){h.o()})}this.h.push(g)};
var e=da.setTimeout;c.prototype.j=function(g){e(g,0)};
c.prototype.o=function(){for(;this.h&&this.h.length;){var g=this.h;this.h=[];for(var h=0;h<g.length;++h){var k=g[h];g[h]=null;try{k()}catch(m){this.l(m)}}}this.h=null};
c.prototype.l=function(g){this.j(function(){throw g;})};
b.prototype.l=function(){function g(m){return function(q){k||(k=!0,m.call(h,q))}}
var h=this,k=!1;return{resolve:g(this.S),reject:g(this.o)}};
b.prototype.S=function(g){if(g===this)this.o(new TypeError("A Promise cannot resolve to itself"));else if(g instanceof b)this.fa(g);else{a:switch(typeof g){case "object":var h=null!=g;break a;case "function":h=!0;break a;default:h=!1}h?this.M(g):this.m(g)}};
b.prototype.M=function(g){var h=void 0;try{h=g.then}catch(k){this.o(k);return}"function"==typeof h?this.na(h,g):this.m(g)};
b.prototype.o=function(g){this.u(2,g)};
b.prototype.m=function(g){this.u(1,g)};
b.prototype.u=function(g,h){if(0!=this.h)throw Error("Cannot settle("+g+", "+h+"): Promise already settled in state"+this.h);this.h=g;this.j=h;2===this.h&&this.Y();this.A()};
b.prototype.Y=function(){var g=this;e(function(){if(g.L()){var h=da.console;"undefined"!==typeof h&&h.error(g.j)}},1)};
b.prototype.L=function(){if(this.C)return!1;var g=da.CustomEvent,h=da.Event,k=da.dispatchEvent;if("undefined"===typeof k)return!0;"function"===typeof g?g=new g("unhandledrejection",{cancelable:!0}):"function"===typeof h?g=new h("unhandledrejection",{cancelable:!0}):(g=da.document.createEvent("CustomEvent"),g.initCustomEvent("unhandledrejection",!1,!0,g));g.promise=this;g.reason=this.j;return k(g)};
b.prototype.A=function(){if(null!=this.i){for(var g=0;g<this.i.length;++g)f.i(this.i[g]);this.i=null}};
var f=new c;b.prototype.fa=function(g){var h=this.l();g.Ia(h.resolve,h.reject)};
b.prototype.na=function(g,h){var k=this.l();try{g.call(h,k.resolve,k.reject)}catch(m){k.reject(m)}};
b.prototype.then=function(g,h){function k(C,v){return"function"==typeof C?function(E){try{m(C(E))}catch(F){q(F)}}:v}
var m,q,u=new b(function(C,v){m=C;q=v});
this.Ia(k(g,m),k(h,q));return u};
b.prototype.catch=function(g){return this.then(void 0,g)};
b.prototype.Ia=function(g,h){function k(){switch(m.h){case 1:g(m.j);break;case 2:h(m.j);break;default:throw Error("Unexpected state: "+m.h);}}
var m=this;null==this.i?f.i(k):this.i.push(k);this.C=!0};
b.resolve=d;b.reject=function(g){return new b(function(h,k){k(g)})};
b.race=function(g){return new b(function(h,k){for(var m=p(g),q=m.next();!q.done;q=m.next())d(q.value).Ia(h,k)})};
b.all=function(g){var h=p(g),k=h.next();return k.done?d([]):new b(function(m,q){function u(E){return function(F){C[E]=F;v--;0==v&&m(C)}}
var C=[],v=0;do C.push(void 0),v++,d(k.value).Ia(u(C.length-1),q),k=h.next();while(!k.done)})};
return b});
n("WeakMap",function(a){function b(k){this.h=(h+=Math.random()+1).toString();if(k){k=p(k);for(var m;!(m=k.next()).done;)m=m.value,this.set(m[0],m[1])}}
function c(){}
function d(k){var m=typeof k;return"object"===m&&null!==k||"function"===m}
function e(k){if(!ha(k,g)){var m=new c;ba(k,g,{value:m})}}
function f(k){var m=Object[k];m&&(Object[k]=function(q){if(q instanceof c)return q;Object.isExtensible(q)&&e(q);return m(q)})}
if(function(){if(!a||!Object.seal)return!1;try{var k=Object.seal({}),m=Object.seal({}),q=new a([[k,2],[m,3]]);if(2!=q.get(k)||3!=q.get(m))return!1;q.delete(k);q.set(m,4);return!q.has(k)&&4==q.get(m)}catch(u){return!1}}())return a;
var g="$jscomp_hidden_"+Math.random();f("freeze");f("preventExtensions");f("seal");var h=0;b.prototype.set=function(k,m){if(!d(k))throw Error("Invalid WeakMap key");e(k);if(!ha(k,g))throw Error("WeakMap key fail: "+k);k[g][this.h]=m;return this};
b.prototype.get=function(k){return d(k)&&ha(k,g)?k[g][this.h]:void 0};
b.prototype.has=function(k){return d(k)&&ha(k,g)&&ha(k[g],this.h)};
b.prototype.delete=function(k){return d(k)&&ha(k,g)&&ha(k[g],this.h)?delete k[g][this.h]:!1};
return b});
n("Map",function(a){function b(){var h={};return h.previous=h.next=h.head=h}
function c(h,k){var m=h.h;return ea(function(){if(m){for(;m.head!=h.h;)m=m.previous;for(;m.next!=m.head;)return m=m.next,{done:!1,value:k(m)};m=null}return{done:!0,value:void 0}})}
function d(h,k){var m=k&&typeof k;"object"==m||"function"==m?f.has(k)?m=f.get(k):(m=""+ ++g,f.set(k,m)):m="p_"+k;var q=h.data_[m];if(q&&ha(h.data_,m))for(h=0;h<q.length;h++){var u=q[h];if(k!==k&&u.key!==u.key||k===u.key)return{id:m,list:q,index:h,entry:u}}return{id:m,list:q,index:-1,entry:void 0}}
function e(h){this.data_={};this.h=b();this.size=0;if(h){h=p(h);for(var k;!(k=h.next()).done;)k=k.value,this.set(k[0],k[1])}}
if(function(){if(!a||"function"!=typeof a||!a.prototype.entries||"function"!=typeof Object.seal)return!1;try{var h=Object.seal({x:4}),k=new a(p([[h,"s"]]));if("s"!=k.get(h)||1!=k.size||k.get({x:4})||k.set({x:4},"t")!=k||2!=k.size)return!1;var m=k.entries(),q=m.next();if(q.done||q.value[0]!=h||"s"!=q.value[1])return!1;q=m.next();return q.done||4!=q.value[0].x||"t"!=q.value[1]||!m.next().done?!1:!0}catch(u){return!1}}())return a;
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
function La(a,b){a instanceof String&&(a+="");var c=0,d=!1,e={next:function(){if(!d&&c<a.length){var f=c++;return{value:b(f,a[f]),done:!1}}d=!0;return{done:!0,value:void 0}}};
e[Symbol.iterator]=function(){return e};
return e}
n("Array.prototype.entries",function(a){return a?a:function(){return La(this,function(b,c){return[b,c]})}});
n("Object.setPrototypeOf",function(a){return a||qa});
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
n("Object.entries",function(a){return a?a:function(b){var c=[],d;for(d in b)ha(b,d)&&c.push([d,b[d]]);return c}});
n("Array.prototype.keys",function(a){return a?a:function(){return La(this,function(b){return b})}});
n("Array.prototype.values",function(a){return a?a:function(){return La(this,function(b,c){return c})}});
n("Object.is",function(a){return a?a:function(b,c){return b===c?0!==b||1/b===1/c:b!==b&&c!==c}});
n("Array.prototype.includes",function(a){return a?a:function(b,c){var d=this;d instanceof String&&(d=String(d));var e=d.length;c=c||0;for(0>c&&(c=Math.max(c+e,0));c<e;c++){var f=d[c];if(f===b||Object.is(f,b))return!0}return!1}});
n("String.prototype.includes",function(a){return a?a:function(b,c){return-1!==Ka(this,b,"includes").indexOf(b,c||0)}});
n("Array.from",function(a){return a?a:function(b,c,d){c=null!=c?c:function(h){return h};
var e=[],f="undefined"!=typeof Symbol&&Symbol.iterator&&b[Symbol.iterator];if("function"==typeof f){b=f.call(b);for(var g=0;!(f=b.next()).done;)e.push(c.call(d,f.value,g++))}else for(f=b.length,g=0;g<f;g++)e.push(c.call(d,b[g],g));return e}});
n("Number.isNaN",function(a){return a?a:function(b){return"number"===typeof b&&isNaN(b)}});
n("Number.MAX_SAFE_INTEGER",function(){return 9007199254740991});
n("Object.values",function(a){return a?a:function(b){var c=[],d;for(d in b)ha(b,d)&&c.push(b[d]);return c}});
var x=this||self;function y(a,b,c){a=a.split(".");c=c||x;a[0]in c||"undefined"==typeof c.execScript||c.execScript("var "+a[0]);for(var d;a.length&&(d=a.shift());)a.length||void 0===b?c[d]&&c[d]!==Object.prototype[d]?c=c[d]:c=c[d]={}:c[d]=b}
function z(a,b){a=a.split(".");b=b||x;for(var c=0;c<a.length;c++)if(b=b[a[c]],null==b)return null;return b}
function Ma(){}
function Oa(a){a.cb=void 0;a.getInstance=function(){return a.cb?a.cb:a.cb=new a}}
function Pa(a){var b=typeof a;return"object"!=b?b:a?Array.isArray(a)?"array":b:"null"}
function Qa(a){var b=Pa(a);return"array"==b||"object"==b&&"number"==typeof a.length}
function Ra(a){var b=typeof a;return"object"==b&&null!=a||"function"==b}
function Sa(a){return Object.prototype.hasOwnProperty.call(a,Ta)&&a[Ta]||(a[Ta]=++Ua)}
var Ta="closure_uid_"+(1E9*Math.random()>>>0),Ua=0;function Va(a,b,c){return a.call.apply(a.bind,arguments)}
function Wa(a,b,c){if(!a)throw Error();if(2<arguments.length){var d=Array.prototype.slice.call(arguments,2);return function(){var e=Array.prototype.slice.call(arguments);Array.prototype.unshift.apply(e,d);return a.apply(b,e)}}return function(){return a.apply(b,arguments)}}
function Xa(a,b,c){Function.prototype.bind&&-1!=Function.prototype.bind.toString().indexOf("native code")?Xa=Va:Xa=Wa;return Xa.apply(null,arguments)}
function Ya(a,b){var c=Array.prototype.slice.call(arguments,1);return function(){var d=c.slice();d.push.apply(d,arguments);return a.apply(this,d)}}
function Za(a,b){y(a,b,void 0)}
function $a(a,b){function c(){}
c.prototype=b.prototype;a.aa=b.prototype;a.prototype=new c;a.prototype.constructor=a;a.Do=function(d,e,f){for(var g=Array(arguments.length-2),h=2;h<arguments.length;h++)g[h-2]=arguments[h];return b.prototype[e].apply(d,g)}}
function ab(a){return a}
;function bb(a,b){if(Error.captureStackTrace)Error.captureStackTrace(this,bb);else{var c=Error().stack;c&&(this.stack=c)}a&&(this.message=String(a));void 0!==b&&(this.Pb=b)}
$a(bb,Error);bb.prototype.name="CustomError";function cb(a){a=a.url;var b=/[?&]dsh=1(&|$)/.test(a);this.j=!b&&/[?&]ae=1(&|$)/.test(a);this.l=!b&&/[?&]ae=2(&|$)/.test(a);if((this.h=/[?&]adurl=([^&]*)/.exec(a))&&this.h[1]){try{var c=decodeURIComponent(this.h[1])}catch(d){c=null}this.i=c}}
;function db(a){var b=!1,c;return function(){b||(c=a(),b=!0);return c}}
;var eb=Array.prototype.indexOf?function(a,b){return Array.prototype.indexOf.call(a,b,void 0)}:function(a,b){if("string"===typeof a)return"string"!==typeof b||1!=b.length?-1:a.indexOf(b,0);
for(var c=0;c<a.length;c++)if(c in a&&a[c]===b)return c;return-1},fb=Array.prototype.forEach?function(a,b,c){Array.prototype.forEach.call(a,b,c)}:function(a,b,c){for(var d=a.length,e="string"===typeof a?a.split(""):a,f=0;f<d;f++)f in e&&b.call(c,e[f],f,a)},gb=Array.prototype.filter?function(a,b){return Array.prototype.filter.call(a,b,void 0)}:function(a,b){for(var c=a.length,d=[],e=0,f="string"===typeof a?a.split(""):a,g=0;g<c;g++)if(g in f){var h=f[g];
b.call(void 0,h,g,a)&&(d[e++]=h)}return d},hb=Array.prototype.map?function(a,b){return Array.prototype.map.call(a,b,void 0)}:function(a,b){for(var c=a.length,d=Array(c),e="string"===typeof a?a.split(""):a,f=0;f<c;f++)f in e&&(d[f]=b.call(void 0,e[f],f,a));
return d},ib=Array.prototype.reduce?function(a,b,c){return Array.prototype.reduce.call(a,b,c)}:function(a,b,c){var d=c;
fb(a,function(e,f){d=b.call(void 0,d,e,f,a)});
return d};
function jb(a,b){a:{for(var c=a.length,d="string"===typeof a?a.split(""):a,e=0;e<c;e++)if(e in d&&b.call(void 0,d[e],e,a)){b=e;break a}b=-1}return 0>b?null:"string"===typeof a?a.charAt(b):a[b]}
function kb(a,b){b=eb(a,b);var c;(c=0<=b)&&Array.prototype.splice.call(a,b,1);return c}
function lb(a,b){for(var c=1;c<arguments.length;c++){var d=arguments[c];if(Qa(d)){var e=a.length||0,f=d.length||0;a.length=e+f;for(var g=0;g<f;g++)a[e+g]=d[g]}else a.push(d)}}
;function mb(a,b){for(var c in a)b.call(void 0,a[c],c,a)}
function nb(a){var b=ob,c;for(c in b)if(a.call(void 0,b[c],c,b))return c}
function pb(a){for(var b in a)return!1;return!0}
function qb(a,b){if(null!==a&&b in a)throw Error('The object already contains the key "'+b+'"');a[b]=!0}
function rb(){var a=A("PLAYER_VARS",{});return null!==a&&"privembed"in a?a.privembed:!1}
function ub(a,b){for(var c in a)if(!(c in b)||a[c]!==b[c])return!1;for(var d in b)if(!(d in a))return!1;return!0}
function vb(a){var b={},c;for(c in a)b[c]=a[c];return b}
function wb(a){if(!a||"object"!==typeof a)return a;if("function"===typeof a.clone)return a.clone();if("undefined"!==typeof Map&&a instanceof Map)return new Map(a);if("undefined"!==typeof Set&&a instanceof Set)return new Set(a);var b=Array.isArray(a)?[]:"function"!==typeof ArrayBuffer||"function"!==typeof ArrayBuffer.isView||!ArrayBuffer.isView(a)||a instanceof DataView?{}:new a.constructor(a.length),c;for(c in a)b[c]=wb(a[c]);return b}
var xb="constructor hasOwnProperty isPrototypeOf propertyIsEnumerable toLocaleString toString valueOf".split(" ");function yb(a,b){for(var c,d,e=1;e<arguments.length;e++){d=arguments[e];for(c in d)a[c]=d[c];for(var f=0;f<xb.length;f++)c=xb[f],Object.prototype.hasOwnProperty.call(d,c)&&(a[c]=d[c])}}
;var zb;function Ab(){if(void 0===zb){var a=null,b=x.trustedTypes;if(b&&b.createPolicy){try{a=b.createPolicy("goog#html",{createHTML:ab,createScript:ab,createScriptURL:ab})}catch(c){x.console&&x.console.error(c.message)}zb=a}else zb=a}return zb}
;function Bb(a,b){this.j=a===Cb&&b||""}
Bb.prototype.i=!0;Bb.prototype.h=function(){return this.j};
function Db(a){return new Bb(Cb,a)}
var Cb={};Db("");var Eb={};function Fb(a){this.j=Eb===Eb?a:"";this.i=!0}
Fb.prototype.h=function(){return this.j.toString()};
Fb.prototype.toString=function(){return this.j.toString()};function Gb(a,b){this.j=b===Hb?a:""}
Gb.prototype.i=!0;Gb.prototype.h=function(){return this.j.toString()};
Gb.prototype.toString=function(){return this.j+""};
function Ib(a){if(a instanceof Gb&&a.constructor===Gb)return a.j;Pa(a);return"type_error:TrustedResourceUrl"}
var Hb={};function Jb(a){var b=Ab();a=b?b.createScriptURL(a):a;return new Gb(a,Hb)}
;var Kb=String.prototype.trim?function(a){return a.trim()}:function(a){return/^[\s\xa0]*([\s\S]*?)[\s\xa0]*$/.exec(a)[1]};function Lb(a,b){this.j=b===Mb?a:""}
Lb.prototype.i=!0;Lb.prototype.h=function(){return this.j.toString()};
Lb.prototype.toString=function(){return this.j.toString()};
function Pb(a){if(a instanceof Lb&&a.constructor===Lb)return a.j;Pa(a);return"type_error:SafeUrl"}
var Qb=/^(?:(?:https?|mailto|ftp):|[^:/?#]*(?:[/?#]|$))/i,Mb={};function Rb(){var a=x.navigator;return a&&(a=a.userAgent)?a:""}
function B(a){return-1!=Rb().indexOf(a)}
;function Sb(){return(B("Chrome")||B("CriOS"))&&!B("Edge")||B("Silk")}
;var Tb={};function Ub(a){this.j=Tb===Tb?a:"";this.i=!0}
Ub.prototype.h=function(){return this.j.toString()};
Ub.prototype.toString=function(){return this.j.toString()};function Vb(a,b){b instanceof Lb||b instanceof Lb||(b="object"==typeof b&&b.i?b.h():String(b),Qb.test(b)||(b="about:invalid#zClosurez"),b=new Lb(b,Mb));a.href=Pb(b)}
function Wb(a,b){a.rel="stylesheet";a.href=Ib(b).toString();(b=Xb('style[nonce],link[rel="stylesheet"][nonce]',a.ownerDocument&&a.ownerDocument.defaultView))&&a.setAttribute("nonce",b)}
function Yb(){return Xb("script[nonce]",void 0)}
var Zb=/^[\w+/_-]+[=]{0,2}$/;function Xb(a,b){b=(b||x).document;return b.querySelector?(a=b.querySelector(a))&&(a=a.nonce||a.getAttribute("nonce"))&&Zb.test(a)?a:"":""}
;function $b(a){for(var b=0,c=0;c<a.length;++c)b=31*b+a.charCodeAt(c)>>>0;return b}
;var ac=RegExp("^(?:([^:/?#.]+):)?(?://(?:([^\\\\/?#]*)@)?([^\\\\/?#]*?)(?::([0-9]+))?(?=[\\\\/?#]|$))?([^?#]+)?(?:\\?([^#]*))?(?:#([\\s\\S]*))?$");function bc(a){return a?decodeURI(a):a}
function cc(a,b){return b.match(ac)[a]||null}
function dc(a){return bc(cc(3,a))}
function ec(a){var b=a.match(ac);a=b[5];var c=b[6];b=b[7];var d="";a&&(d+=a);c&&(d+="?"+c);b&&(d+="#"+b);return d}
function fc(a,b,c){if(Array.isArray(b))for(var d=0;d<b.length;d++)fc(a,String(b[d]),c);else null!=b&&c.push(a+(""===b?"":"="+encodeURIComponent(String(b))))}
function gc(a){var b=[],c;for(c in a)fc(c,a[c],b);return b.join("&")}
function hc(a,b){b=gc(b);if(b){var c=a.indexOf("#");0>c&&(c=a.length);var d=a.indexOf("?");if(0>d||d>c){d=c;var e=""}else e=a.substring(d+1,c);a=[a.substr(0,d),e,a.substr(c)];c=a[1];a[1]=b?c?c+"&"+b:b:c;b=a[0]+(a[1]?"?"+a[1]:"")+a[2]}else b=a;return b}
function ic(a,b,c,d){for(var e=c.length;0<=(b=a.indexOf(c,b))&&b<d;){var f=a.charCodeAt(b-1);if(38==f||63==f)if(f=a.charCodeAt(b+e),!f||61==f||38==f||35==f)return b;b+=e+1}return-1}
var jc=/#|$/,kc=/[?&]($|#)/;function lc(a){for(var b=a.search(jc),c=0,d,e=[];0<=(d=ic(a,c,"key",b));)e.push(a.substring(c,d)),c=Math.min(a.indexOf("&",d)+1||b,b);e.push(a.substr(c));return e.join("").replace(kc,"$1")}
;function mc(){return B("iPhone")&&!B("iPod")&&!B("iPad")}
;function qc(a){qc[" "](a);return a}
qc[" "]=Ma;var rc=B("Opera"),sc=B("Trident")||B("MSIE"),tc=B("Edge"),uc=B("Gecko")&&!(-1!=Rb().toLowerCase().indexOf("webkit")&&!B("Edge"))&&!(B("Trident")||B("MSIE"))&&!B("Edge"),vc=-1!=Rb().toLowerCase().indexOf("webkit")&&!B("Edge"),wc=B("Android");function xc(){var a=x.document;return a?a.documentMode:void 0}
var yc;a:{var zc="",Ac=function(){var a=Rb();if(uc)return/rv:([^\);]+)(\)|;)/.exec(a);if(tc)return/Edge\/([\d\.]+)/.exec(a);if(sc)return/\b(?:MSIE|rv)[: ]([^\);]+)(\)|;)/.exec(a);if(vc)return/WebKit\/(\S+)/.exec(a);if(rc)return/(?:Version)[ \/]?(\S+)/.exec(a)}();
Ac&&(zc=Ac?Ac[1]:"");if(sc){var Bc=xc();if(null!=Bc&&Bc>parseFloat(zc)){yc=String(Bc);break a}}yc=zc}var Cc=yc,Dc;if(x.document&&sc){var Ec=xc();Dc=Ec?Ec:parseInt(Cc,10)||void 0}else Dc=void 0;var Fc=Dc;var Gc=mc()||B("iPod"),Hc=B("iPad");!B("Android")||Sb();Sb();var Ic=B("Safari")&&!(Sb()||B("Coast")||B("Opera")||B("Edge")||B("Edg/")||B("OPR")||B("Firefox")||B("FxiOS")||B("Silk")||B("Android"))&&!(mc()||B("iPad")||B("iPod"));var Jc={},Kc=null;
function Lc(a,b){Qa(a);void 0===b&&(b=0);if(!Kc){Kc={};for(var c="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789".split(""),d=["+/=","+/","-_=","-_.","-_"],e=0;5>e;e++){var f=c.concat(d[e].split(""));Jc[e]=f;for(var g=0;g<f.length;g++){var h=f[g];void 0===Kc[h]&&(Kc[h]=g)}}}b=Jc[b];c=Array(Math.floor(a.length/3));d=b[64]||"";for(e=f=0;f<a.length-2;f+=3){var k=a[f],m=a[f+1];h=a[f+2];g=b[k>>2];k=b[(k&3)<<4|m>>4];m=b[(m&15)<<2|h>>6];h=b[h&63];c[e++]=""+g+k+m+h}g=0;h=d;switch(a.length-
f){case 2:g=a[f+1],h=b[(g&15)<<2]||d;case 1:a=a[f],c[e]=""+b[a>>2]+b[(a&3)<<4|g>>4]+h+d}return c.join("")}
;var Mc="function"===typeof Uint8Array;var Nc="function"===typeof Symbol&&"symbol"===typeof Symbol()?Symbol(void 0):void 0;function Oc(a,b){Object.isFrozen(a)||(Nc?a[Nc]|=b:void 0!==a.Ma?a.Ma|=b:Object.defineProperties(a,{Ma:{value:b,configurable:!0,writable:!0,enumerable:!1}}))}
function Pc(a){var b;Nc?b=a[Nc]:b=a.Ma;return null==b?0:b}
function Qc(a){Oc(a,1);return a}
function Rc(a){return Array.isArray(a)?!!(Pc(a)&2):!1}
function Sc(a){if(!Array.isArray(a))throw Error("cannot mark non-array as immutable");Oc(a,2)}
;function Tc(a){return null!==a&&"object"===typeof a&&!Array.isArray(a)&&a.constructor===Object}
var Uc,Vc=Object.freeze(Qc([]));function Wc(a){if(Rc(a.G))throw Error("Cannot mutate an immutable Message");}
var Xc="undefined"!=typeof Symbol&&"undefined"!=typeof Symbol.hasInstance;function Yc(a){return{value:a,configurable:!1,writable:!1,enumerable:!1}}
;function Zc(a){switch(typeof a){case "number":return isFinite(a)?a:String(a);case "object":if(a&&!Array.isArray(a)&&Mc&&null!=a&&a instanceof Uint8Array)return Lc(a)}return a}
;function $c(a,b){b=void 0===b?ad:b;return bd(a,b)}
function fd(a,b){if(null!=a){if(Array.isArray(a))a=bd(a,b);else if(Tc(a)){var c={},d;for(d in a)c[d]=fd(a[d],b);a=c}else a=b(a);return a}}
function bd(a,b){for(var c=a.slice(),d=0;d<c.length;d++)c[d]=fd(c[d],b);Array.isArray(a)&&Pc(a)&1&&Qc(c);return c}
function gd(a){if(a&&"object"==typeof a&&a.toJSON)return a.toJSON();a=Zc(a);return Array.isArray(a)?$c(a,gd):a}
function ad(a){return Mc&&null!=a&&a instanceof Uint8Array?new Uint8Array(a):a}
;function hd(a,b,c){return-1===b?null:b>=a.l?a.i?a.i[b]:void 0:(void 0===c?0:c)&&a.i&&(c=a.i[b],null!=c)?c:a.G[b+a.j]}
function D(a,b,c,d,e){d=void 0===d?!1:d;(void 0===e?0:e)||Wc(a);b<a.l&&!d?a.G[b+a.j]=c:(a.i||(a.i=a.G[a.l+a.j]={}))[b]=c;return a}
function id(a,b,c,d){c=void 0===c?!0:c;d=void 0===d?!1:d;var e=hd(a,b,d);null==e&&(e=Vc);if(Rc(a.G))c&&(Sc(e),Object.freeze(e));else if(e===Vc||Rc(e))e=Qc(e.slice()),D(a,b,e,d);return e}
function jd(a,b,c,d){Wc(a);(c=kd(a,c))&&c!==b&&null!=d&&(a.h&&c in a.h&&(a.h[c]=void 0),D(a,c,void 0));return D(a,b,d)}
function kd(a,b){for(var c=0,d=0;d<b.length;d++){var e=b[d];null!=hd(a,e)&&(0!==c&&D(a,c,void 0,!1,!0),c=e)}return c}
function ld(a,b,c,d,e){if(-1===c)return null;a.h||(a.h={});var f=a.h[c];if(f)return f;e=hd(a,c,void 0===e?!1:e);if(null==e&&!d)return f;b=new b(e);Rc(a.G)&&Sc(b.G);return a.h[c]=b}
function md(a,b,c,d){d=void 0===d?!1:d;a.h||(a.h={});var e=Rc(a.G),f=a.h[c];if(!f){d=id(a,c,!0,d);f=[];e=e||Rc(d);for(var g=0;g<d.length;g++)f[g]=new b(d[g]),e&&Sc(f[g].G);e&&(Sc(f),Object.freeze(f));a.h[c]=f}return f}
function I(a,b,c,d){d=void 0===d?!1:d;Wc(a);a.h||(a.h={});var e=c?c.G:c;a.h[b]=c;return D(a,b,e,d)}
function nd(a,b,c,d){Wc(a);a.h||(a.h={});var e=d?d.G:d;a.h[b]=d;jd(a,b,c,e)}
function od(a,b,c,d){var e=void 0===e?!1:e;Wc(a);e=md(a,c,b,e);c=d?d:new c;a=id(a,b);e.push(c);a.push(c.G)}
function pd(a,b){a=hd(a,b);return null==a?"":a}
;function qd(a){x.setTimeout(function(){throw a;},0)}
;function rd(a,b,c){a||(a=sd);sd=null;var d=this.constructor.i;a||(a=d?[d]:[]);this.j=(d?0:-1)-(this.constructor.h||0);this.h=void 0;this.G=a;a:{d=this.G.length;a=d-1;if(d&&(d=this.G[a],Tc(d))){this.l=a-this.j;this.i=d;break a}void 0!==b&&-1<b?(this.l=Math.max(b,a+1-this.j),this.i=void 0):this.l=Number.MAX_VALUE}if(c)for(b=0;b<c.length;b++)if(a=c[b],a<this.l)a+=this.j,(d=this.G[a])?Array.isArray(d)&&Qc(d):this.G[a]=Vc;else{d=this.i||(this.i=this.G[this.l+this.j]={});var e=d[a];e?Array.isArray(e)&&
Qc(e):d[a]=Vc}}
rd.prototype.toJSON=function(){var a=this.G;return Uc?a:$c(a,gd)};
function td(a){Uc=!0;try{return JSON.stringify(a.toJSON(),ud)}finally{Uc=!1}}
rd.prototype.clone=function(){var a=$c(this.G);sd=a;a=new this.constructor(a);sd=null;vd(a,this);return a};
rd.prototype.toString=function(){return this.G.toString()};
function ud(a,b){return Zc(b)}
function vd(a,b){b.o&&(a.o=b.o.slice());var c=b.h;if(c){b=b.i;for(var d in c){var e=c[d];if(e){var f=!(!b||!b[d]),g=+d;if(Array.isArray(e)){if(e.length)for(f=md(a,e[0].constructor,g,f),g=0;g<Math.min(f.length,e.length);g++)vd(f[g],e[g])}else(f=ld(a,e.constructor,g,void 0,f))&&vd(f,e)}}}}
var sd;function wd(){rd.apply(this,arguments)}
r(wd,rd);if(Xc){var xd={};Object.defineProperties(wd,(xd[Symbol.hasInstance]=Yc(function(){throw Error("Cannot perform instanceof checks for MutableMessage");}),xd))};function yd(a,b){var c=this.h;if(this.isRepeated){var d=!0;d=void 0===d?!1:d;Wc(a);if(b){var e=Qc([]);for(var f=0;f<b.length;f++)e[f]=b[f].G;a.h||(a.h={});a.h[c]=b}else a.h&&(a.h[c]=void 0),e=Vc;a=D(a,c,e,d)}else a=I(a,c,b,!0);return a}
;function J(){wd.apply(this,arguments)}
r(J,wd);if(Xc){var zd={};Object.defineProperties(J,(zd[Symbol.hasInstance]=Yc(Object[Symbol.hasInstance]),zd))};function Ad(){var a=this;this.promise=new Promise(function(b,c){a.resolve=b;a.reject=c})}
;function Bd(a){this.i=!1;var b=a.program;a=a.globalName;var c=new Ad;this.j=c.promise;this.l=p((0,x[a].a)(b,function(d,e){Promise.resolve().then(function(){c.resolve({Ob:d,xc:e})})},!0)).next().value;
this.wc=c.promise.then(function(){})}
Bd.prototype.snapshot=function(a){if(this.i)throw Error("Already disposed");return this.j.then(function(b){var c=b.Ob;return new Promise(function(d){c(function(e){d(e)},[a.pb,
a.yc])})})};
Bd.prototype.dispose=function(){this.i=!0;this.j.then(function(a){(a=a.xc)&&a()})};
Bd.prototype.h=function(){return this.i};var Cd=window;Db("csi.gstatic.com");Db("googleads.g.doubleclick.net");Db("partner.googleadservices.com");Db("pubads.g.doubleclick.net");Db("securepubads.g.doubleclick.net");Db("tpc.googlesyndication.com");/*

 SPDX-License-Identifier: Apache-2.0
*/
var Dd={};function Ed(){}
function Fd(a){this.h=a}
r(Fd,Ed);Fd.prototype.toString=function(){return this.h};
var Gd=new Fd("about:invalid#zTSz",Dd);function Hd(a){if(a instanceof Ed)if(a instanceof Fd)a=a.h;else throw Error("");else a=Pb(a);return a}
;function Id(a,b){a.src=Ib(b);var c,d;(c=(b=null==(d=(c=(a.ownerDocument&&a.ownerDocument.defaultView||window).document).querySelector)?void 0:d.call(c,"script[nonce]"))?b.nonce||b.getAttribute("nonce")||"":"")&&a.setAttribute("nonce",c)}
;function Jd(a,b){this.x=void 0!==a?a:0;this.y=void 0!==b?b:0}
l=Jd.prototype;l.clone=function(){return new Jd(this.x,this.y)};
l.equals=function(a){return a instanceof Jd&&(this==a?!0:this&&a?this.x==a.x&&this.y==a.y:!1)};
l.ceil=function(){this.x=Math.ceil(this.x);this.y=Math.ceil(this.y);return this};
l.floor=function(){this.x=Math.floor(this.x);this.y=Math.floor(this.y);return this};
l.round=function(){this.x=Math.round(this.x);this.y=Math.round(this.y);return this};
l.scale=function(a,b){this.x*=a;this.y*="number"===typeof b?b:a;return this};function Kd(a,b){this.width=a;this.height=b}
l=Kd.prototype;l.clone=function(){return new Kd(this.width,this.height)};
l.aspectRatio=function(){return this.width/this.height};
l.isEmpty=function(){return!(this.width*this.height)};
l.ceil=function(){this.width=Math.ceil(this.width);this.height=Math.ceil(this.height);return this};
l.floor=function(){this.width=Math.floor(this.width);this.height=Math.floor(this.height);return this};
l.round=function(){this.width=Math.round(this.width);this.height=Math.round(this.height);return this};
l.scale=function(a,b){this.width*=a;this.height*="number"===typeof b?b:a;return this};function Ld(a){var b=document;return"string"===typeof a?b.getElementById(a):a}
function Md(a){var b=document;a=String(a);"application/xhtml+xml"===b.contentType&&(a=a.toLowerCase());return b.createElement(a)}
function Nd(a,b){for(var c=0;a;){if(b(a))return a;a=a.parentNode;c++}return null}
;function Od(a){var b=Pd;if(b)for(var c in b)Object.prototype.hasOwnProperty.call(b,c)&&a(b[c],c,b)}
function Qd(){var a=[];Od(function(b){a.push(b)});
return a}
var Pd={Pc:"allow-forms",Qc:"allow-modals",Rc:"allow-orientation-lock",Sc:"allow-pointer-lock",Tc:"allow-popups",Uc:"allow-popups-to-escape-sandbox",Vc:"allow-presentation",Wc:"allow-same-origin",Xc:"allow-scripts",Yc:"allow-top-navigation",Zc:"allow-top-navigation-by-user-activation"},Rd=db(function(){return Qd()});
function Sd(){var a=Td(),b={};fb(Rd(),function(c){a.sandbox&&a.sandbox.supports&&a.sandbox.supports(c)&&(b[c]=!0)});
return b}
function Td(){var a=void 0===a?document:a;return a.createElement("iframe")}
;function Ud(a){this.isValid=a}
function Vd(a){return new Ud(function(b){return b.substr(0,a.length+1).toLowerCase()===a+":"})}
var Wd=[Vd("data"),Vd("http"),Vd("https"),Vd("mailto"),Vd("ftp"),new Ud(function(a){return/^[^:]*([/?#]|$)/.test(a)})];function Xd(a){"number"==typeof a&&(a=Math.round(a)+"px");return a}
;var Yd=(new Date).getTime();function Zd(a){if(!a)return"";if(/^about:(?:blank|srcdoc)$/.test(a))return window.origin||"";a=a.split("#")[0].split("?")[0];a=a.toLowerCase();0==a.indexOf("//")&&(a=window.location.protocol+a);/^[\w\-]*:\/\//.test(a)||(a=window.location.href);var b=a.substring(a.indexOf("://")+3),c=b.indexOf("/");-1!=c&&(b=b.substring(0,c));c=a.substring(0,a.indexOf("://"));if(!c)throw Error("URI is missing protocol: "+a);if("http"!==c&&"https"!==c&&"chrome-extension"!==c&&"moz-extension"!==c&&"file"!==c&&"android-app"!==
c&&"chrome-search"!==c&&"chrome-untrusted"!==c&&"chrome"!==c&&"app"!==c&&"devtools"!==c)throw Error("Invalid URI scheme in origin: "+c);a="";var d=b.indexOf(":");if(-1!=d){var e=b.substring(d+1);b=b.substring(0,d);if("http"===c&&"80"!==e||"https"===c&&"443"!==e)a=":"+e}return c+"://"+b+a}
;function $d(){function a(){e[0]=1732584193;e[1]=4023233417;e[2]=2562383102;e[3]=271733878;e[4]=3285377520;q=m=0}
function b(u){for(var C=g,v=0;64>v;v+=4)C[v/4]=u[v]<<24|u[v+1]<<16|u[v+2]<<8|u[v+3];for(v=16;80>v;v++)u=C[v-3]^C[v-8]^C[v-14]^C[v-16],C[v]=(u<<1|u>>>31)&4294967295;u=e[0];var E=e[1],F=e[2],H=e[3],R=e[4];for(v=0;80>v;v++){if(40>v)if(20>v){var O=H^E&(F^H);var S=1518500249}else O=E^F^H,S=1859775393;else 60>v?(O=E&F|H&(E|F),S=2400959708):(O=E^F^H,S=3395469782);O=((u<<5|u>>>27)&4294967295)+O+R+S+C[v]&4294967295;R=H;H=F;F=(E<<30|E>>>2)&4294967295;E=u;u=O}e[0]=e[0]+u&4294967295;e[1]=e[1]+E&4294967295;e[2]=
e[2]+F&4294967295;e[3]=e[3]+H&4294967295;e[4]=e[4]+R&4294967295}
function c(u,C){if("string"===typeof u){u=unescape(encodeURIComponent(u));for(var v=[],E=0,F=u.length;E<F;++E)v.push(u.charCodeAt(E));u=v}C||(C=u.length);v=0;if(0==m)for(;v+64<C;)b(u.slice(v,v+64)),v+=64,q+=64;for(;v<C;)if(f[m++]=u[v++],q++,64==m)for(m=0,b(f);v+64<C;)b(u.slice(v,v+64)),v+=64,q+=64}
function d(){var u=[],C=8*q;56>m?c(h,56-m):c(h,64-(m-56));for(var v=63;56<=v;v--)f[v]=C&255,C>>>=8;b(f);for(v=C=0;5>v;v++)for(var E=24;0<=E;E-=8)u[C++]=e[v]>>E&255;return u}
for(var e=[],f=[],g=[],h=[128],k=1;64>k;++k)h[k]=0;var m,q;a();return{reset:a,update:c,digest:d,Sb:function(){for(var u=d(),C="",v=0;v<u.length;v++)C+="0123456789ABCDEF".charAt(Math.floor(u[v]/16))+"0123456789ABCDEF".charAt(u[v]%16);return C}}}
;function de(a,b,c){var d=String(x.location.href);return d&&a&&b?[b,ee(Zd(d),a,c||null)].join(" "):null}
function ee(a,b,c){var d=[],e=[];if(1==(Array.isArray(c)?2:1))return e=[b,a],fb(d,function(h){e.push(h)}),fe(e.join(" "));
var f=[],g=[];fb(c,function(h){g.push(h.key);f.push(h.value)});
c=Math.floor((new Date).getTime()/1E3);e=0==f.length?[c,b,a]:[f.join(":"),c,b,a];fb(d,function(h){e.push(h)});
a=fe(e.join(" "));a=[c,a];0==g.length||a.push(g.join(""));return a.join("_")}
function fe(a){var b=$d();b.update(a);return b.Sb().toLowerCase()}
;var ge={};function he(a){this.h=a||{cookie:""}}
l=he.prototype;l.isEnabled=function(){if(!x.navigator.cookieEnabled)return!1;if(!this.isEmpty())return!0;this.set("TESTCOOKIESENABLED","1",{Oa:60});if("1"!==this.get("TESTCOOKIESENABLED"))return!1;this.remove("TESTCOOKIESENABLED");return!0};
l.set=function(a,b,c){var d=!1;if("object"===typeof c){var e=c.Po;d=c.secure||!1;var f=c.domain||void 0;var g=c.path||void 0;var h=c.Oa}if(/[;=\s]/.test(a))throw Error('Invalid cookie name "'+a+'"');if(/[;\r\n]/.test(b))throw Error('Invalid cookie value "'+b+'"');void 0===h&&(h=-1);c=f?";domain="+f:"";g=g?";path="+g:"";d=d?";secure":"";h=0>h?"":0==h?";expires="+(new Date(1970,1,1)).toUTCString():";expires="+(new Date(Date.now()+1E3*h)).toUTCString();this.h.cookie=a+"="+b+c+g+h+d+(null!=e?";samesite="+
e:"")};
l.get=function(a,b){for(var c=a+"=",d=(this.h.cookie||"").split(";"),e=0,f;e<d.length;e++){f=Kb(d[e]);if(0==f.lastIndexOf(c,0))return f.substr(c.length);if(f==a)return""}return b};
l.remove=function(a,b,c){var d=void 0!==this.get(a);this.set(a,"",{Oa:0,path:b,domain:c});return d};
l.Ya=function(){return ie(this).keys};
l.isEmpty=function(){return!this.h.cookie};
l.clear=function(){for(var a=ie(this).keys,b=a.length-1;0<=b;b--)this.remove(a[b])};
function ie(a){a=(a.h.cookie||"").split(";");for(var b=[],c=[],d,e,f=0;f<a.length;f++)e=Kb(a[f]),d=e.indexOf("="),-1==d?(b.push(""),c.push(e)):(b.push(e.substring(0,d)),c.push(e.substring(d+1)));return{keys:b,values:c}}
var je=new he("undefined"==typeof document?null:document);function ke(a){return!!ge.FPA_SAMESITE_PHASE2_MOD||!(void 0===a||!a)}
function le(a){a=void 0===a?!1:a;var b=x.__SAPISID||x.__APISID||x.__3PSAPISID||x.__OVERRIDE_SID;ke(a)&&(b=b||x.__1PSAPISID);if(b)return!0;var c=new he(document);b=c.get("SAPISID")||c.get("APISID")||c.get("__Secure-3PAPISID")||c.get("SID");ke(a)&&(b=b||c.get("__Secure-1PAPISID"));return!!b}
function me(a,b,c,d){(a=x[a])||(a=(new he(document)).get(b));return a?de(a,c,d):null}
function ne(a){var b=void 0===b?!1:b;var c=Zd(String(x.location.href)),d=[];if(le(b)){c=0==c.indexOf("https:")||0==c.indexOf("chrome-extension:")||0==c.indexOf("moz-extension:");var e=c?x.__SAPISID:x.__APISID;e||(e=new he(document),e=e.get(c?"SAPISID":"APISID")||e.get("__Secure-3PAPISID"));(e=e?de(e,c?"SAPISIDHASH":"APISIDHASH",a):null)&&d.push(e);c&&ke(b)&&((b=me("__1PSAPISID","__Secure-1PAPISID","SAPISID1PHASH",a))&&d.push(b),(a=me("__3PSAPISID","__Secure-3PAPISID","SAPISID3PHASH",a))&&d.push(a))}return 0==
d.length?null:d.join(" ")}
;function oe(a){if(Xc){var b={};Object.defineProperties(a,(b[Symbol.hasInstance]=Yc(Object[Symbol.hasInstance]),b))}}
;function pe(a){a&&"function"==typeof a.dispose&&a.dispose()}
;function qe(a){for(var b=0,c=arguments.length;b<c;++b){var d=arguments[b];Qa(d)?qe.apply(null,d):pe(d)}}
;function K(){this.C=this.C;this.o=this.o}
K.prototype.C=!1;K.prototype.h=function(){return this.C};
K.prototype.dispose=function(){this.C||(this.C=!0,this.I())};
function re(a,b){a.C?b():(a.o||(a.o=[]),a.o.push(b))}
K.prototype.I=function(){if(this.o)for(;this.o.length;)this.o.shift()()};function se(a,b){this.type=a;this.h=this.target=b;this.defaultPrevented=this.j=!1}
se.prototype.stopPropagation=function(){this.j=!0};
se.prototype.preventDefault=function(){this.defaultPrevented=!0};function te(a){var b=z("window.location.href");null==a&&(a='Unknown Error of type "null/undefined"');if("string"===typeof a)return{message:a,name:"Unknown error",lineNumber:"Not available",fileName:b,stack:"Not available"};var c=!1;try{var d=a.lineNumber||a.line||"Not available"}catch(g){d="Not available",c=!0}try{var e=a.fileName||a.filename||a.sourceURL||x.$googDebugFname||b}catch(g){e="Not available",c=!0}b=ue(a);if(!(!c&&a.lineNumber&&a.fileName&&a.stack&&a.message&&a.name)){c=a.message;if(null==
c){if(a.constructor&&a.constructor instanceof Function){if(a.constructor.name)c=a.constructor.name;else if(c=a.constructor,ve[c])c=ve[c];else{c=String(c);if(!ve[c]){var f=/function\s+([^\(]+)/m.exec(c);ve[c]=f?f[1]:"[Anonymous]"}c=ve[c]}c='Unknown Error of type "'+c+'"'}else c="Unknown Error of unknown type";"function"===typeof a.toString&&Object.prototype.toString!==a.toString&&(c+=": "+a.toString())}return{message:c,name:a.name||"UnknownError",lineNumber:d,fileName:e,stack:b||"Not available"}}a.stack=
b;return{message:a.message,name:a.name,lineNumber:a.lineNumber,fileName:a.fileName,stack:a.stack}}
function ue(a,b){b||(b={});b[we(a)]=!0;var c=a.stack||"";(a=a.Pb)&&!b[we(a)]&&(c+="\nCaused by: ",a.stack&&0==a.stack.indexOf(a.toString())||(c+="string"===typeof a?a:a.message+"\n"),c+=ue(a,b));return c}
function we(a){var b="";"function"===typeof a.toString&&(b=""+a);return b+a.stack}
var ve={};var xe=function(){if(!x.addEventListener||!Object.defineProperty)return!1;var a=!1,b=Object.defineProperty({},"passive",{get:function(){a=!0}});
try{x.addEventListener("test",Ma,b),x.removeEventListener("test",Ma,b)}catch(c){}return a}();function ye(a,b){se.call(this,a?a.type:"");this.relatedTarget=this.h=this.target=null;this.button=this.screenY=this.screenX=this.clientY=this.clientX=0;this.key="";this.charCode=this.keyCode=0;this.metaKey=this.shiftKey=this.altKey=this.ctrlKey=!1;this.state=null;this.pointerId=0;this.pointerType="";this.i=null;a&&this.init(a,b)}
$a(ye,se);var ze={2:"touch",3:"pen",4:"mouse"};
ye.prototype.init=function(a,b){var c=this.type=a.type,d=a.changedTouches&&a.changedTouches.length?a.changedTouches[0]:null;this.target=a.target||a.srcElement;this.h=b;if(b=a.relatedTarget){if(uc){a:{try{qc(b.nodeName);var e=!0;break a}catch(f){}e=!1}e||(b=null)}}else"mouseover"==c?b=a.fromElement:"mouseout"==c&&(b=a.toElement);this.relatedTarget=b;d?(this.clientX=void 0!==d.clientX?d.clientX:d.pageX,this.clientY=void 0!==d.clientY?d.clientY:d.pageY,this.screenX=d.screenX||0,this.screenY=d.screenY||
0):(this.clientX=void 0!==a.clientX?a.clientX:a.pageX,this.clientY=void 0!==a.clientY?a.clientY:a.pageY,this.screenX=a.screenX||0,this.screenY=a.screenY||0);this.button=a.button;this.keyCode=a.keyCode||0;this.key=a.key||"";this.charCode=a.charCode||("keypress"==c?a.keyCode:0);this.ctrlKey=a.ctrlKey;this.altKey=a.altKey;this.shiftKey=a.shiftKey;this.metaKey=a.metaKey;this.pointerId=a.pointerId||0;this.pointerType="string"===typeof a.pointerType?a.pointerType:ze[a.pointerType]||"";this.state=a.state;
this.i=a;a.defaultPrevented&&ye.aa.preventDefault.call(this)};
ye.prototype.stopPropagation=function(){ye.aa.stopPropagation.call(this);this.i.stopPropagation?this.i.stopPropagation():this.i.cancelBubble=!0};
ye.prototype.preventDefault=function(){ye.aa.preventDefault.call(this);var a=this.i;a.preventDefault?a.preventDefault():a.returnValue=!1};var Ae="closure_listenable_"+(1E6*Math.random()|0);var Be=0;function Ce(a,b,c,d,e){this.listener=a;this.proxy=null;this.src=b;this.type=c;this.capture=!!d;this.La=e;this.key=++Be;this.ya=this.Ha=!1}
function De(a){a.ya=!0;a.listener=null;a.proxy=null;a.src=null;a.La=null}
;function Ee(a){this.src=a;this.listeners={};this.h=0}
Ee.prototype.add=function(a,b,c,d,e){var f=a.toString();a=this.listeners[f];a||(a=this.listeners[f]=[],this.h++);var g=Fe(a,b,d,e);-1<g?(b=a[g],c||(b.Ha=!1)):(b=new Ce(b,this.src,f,!!d,e),b.Ha=c,a.push(b));return b};
Ee.prototype.remove=function(a,b,c,d){a=a.toString();if(!(a in this.listeners))return!1;var e=this.listeners[a];b=Fe(e,b,c,d);return-1<b?(De(e[b]),Array.prototype.splice.call(e,b,1),0==e.length&&(delete this.listeners[a],this.h--),!0):!1};
function Ge(a,b){var c=b.type;c in a.listeners&&kb(a.listeners[c],b)&&(De(b),0==a.listeners[c].length&&(delete a.listeners[c],a.h--))}
function Fe(a,b,c,d){for(var e=0;e<a.length;++e){var f=a[e];if(!f.ya&&f.listener==b&&f.capture==!!c&&f.La==d)return e}return-1}
;var He="closure_lm_"+(1E6*Math.random()|0),Ie={},Je=0;function Ke(a,b,c,d,e){if(d&&d.once)Le(a,b,c,d,e);else if(Array.isArray(b))for(var f=0;f<b.length;f++)Ke(a,b[f],c,d,e);else c=Me(c),a&&a[Ae]?a.V(b,c,Ra(d)?!!d.capture:!!d,e):Ne(a,b,c,!1,d,e)}
function Ne(a,b,c,d,e,f){if(!b)throw Error("Invalid event type");var g=Ra(e)?!!e.capture:!!e,h=Oe(a);h||(a[He]=h=new Ee(a));c=h.add(b,c,d,g,f);if(!c.proxy){d=Pe();c.proxy=d;d.src=a;d.listener=c;if(a.addEventListener)xe||(e=g),void 0===e&&(e=!1),a.addEventListener(b.toString(),d,e);else if(a.attachEvent)a.attachEvent(Qe(b.toString()),d);else if(a.addListener&&a.removeListener)a.addListener(d);else throw Error("addEventListener and attachEvent are unavailable.");Je++}}
function Pe(){function a(c){return b.call(a.src,a.listener,c)}
var b=Re;return a}
function Le(a,b,c,d,e){if(Array.isArray(b))for(var f=0;f<b.length;f++)Le(a,b[f],c,d,e);else c=Me(c),a&&a[Ae]?a.l.add(String(b),c,!0,Ra(d)?!!d.capture:!!d,e):Ne(a,b,c,!0,d,e)}
function Se(a,b,c,d,e){if(Array.isArray(b))for(var f=0;f<b.length;f++)Se(a,b[f],c,d,e);else(d=Ra(d)?!!d.capture:!!d,c=Me(c),a&&a[Ae])?a.l.remove(String(b),c,d,e):a&&(a=Oe(a))&&(b=a.listeners[b.toString()],a=-1,b&&(a=Fe(b,c,d,e)),(c=-1<a?b[a]:null)&&Te(c))}
function Te(a){if("number"!==typeof a&&a&&!a.ya){var b=a.src;if(b&&b[Ae])Ge(b.l,a);else{var c=a.type,d=a.proxy;b.removeEventListener?b.removeEventListener(c,d,a.capture):b.detachEvent?b.detachEvent(Qe(c),d):b.addListener&&b.removeListener&&b.removeListener(d);Je--;(c=Oe(b))?(Ge(c,a),0==c.h&&(c.src=null,b[He]=null)):De(a)}}}
function Qe(a){return a in Ie?Ie[a]:Ie[a]="on"+a}
function Re(a,b){if(a.ya)a=!0;else{b=new ye(b,this);var c=a.listener,d=a.La||a.src;a.Ha&&Te(a);a=c.call(d,b)}return a}
function Oe(a){a=a[He];return a instanceof Ee?a:null}
var Ue="__closure_events_fn_"+(1E9*Math.random()>>>0);function Me(a){if("function"===typeof a)return a;a[Ue]||(a[Ue]=function(b){return a.handleEvent(b)});
return a[Ue]}
;function Ve(){K.call(this);this.l=new Ee(this);this.Y=this;this.L=null}
$a(Ve,K);Ve.prototype[Ae]=!0;Ve.prototype.addEventListener=function(a,b,c,d){Ke(this,a,b,c,d)};
Ve.prototype.removeEventListener=function(a,b,c,d){Se(this,a,b,c,d)};
function We(a,b){var c=a.L;if(c){var d=[];for(var e=1;c;c=c.L)d.push(c),++e}a=a.Y;c=b.type||b;"string"===typeof b?b=new se(b,a):b instanceof se?b.target=b.target||a:(e=b,b=new se(c,a),yb(b,e));e=!0;if(d)for(var f=d.length-1;!b.j&&0<=f;f--){var g=b.h=d[f];e=Xe(g,c,!0,b)&&e}b.j||(g=b.h=a,e=Xe(g,c,!0,b)&&e,b.j||(e=Xe(g,c,!1,b)&&e));if(d)for(f=0;!b.j&&f<d.length;f++)g=b.h=d[f],e=Xe(g,c,!1,b)&&e}
Ve.prototype.I=function(){Ve.aa.I.call(this);if(this.l){var a=this.l,b=0,c;for(c in a.listeners){for(var d=a.listeners[c],e=0;e<d.length;e++)++b,De(d[e]);delete a.listeners[c];a.h--}}this.L=null};
Ve.prototype.V=function(a,b,c,d){return this.l.add(String(a),b,!1,c,d)};
function Xe(a,b,c,d){b=a.l.listeners[String(b)];if(!b)return!0;b=b.concat();for(var e=!0,f=0;f<b.length;++f){var g=b[f];if(g&&!g.ya&&g.capture==c){var h=g.listener,k=g.La||g.src;g.Ha&&Ge(a.l,g);e=!1!==h.call(k,d)&&e}}return e&&!d.defaultPrevented}
;function Ye(a){Ve.call(this);var b=this;this.A=this.j=0;this.U=null!=a?a:{N:function(e,f){return setTimeout(e,f)},
T:clearTimeout};var c,d;this.i=null!=(d=null==(c=window.navigator)?void 0:c.onLine)?d:!0;this.m=function(){return w(function(e){return t(e,Ze(b),0)})};
window.addEventListener("offline",this.m);window.addEventListener("online",this.m);this.A||$e(this)}
r(Ye,Ve);function af(){var a=bf;Ye.h||(Ye.h=new Ye(a));return Ye.h}
Ye.prototype.dispose=function(){window.removeEventListener("offline",this.m);window.removeEventListener("online",this.m);this.U.T(this.A);delete Ye.h};
Ye.prototype.H=function(){return this.i};
function $e(a){a.A=a.U.N(function(){var b;return w(function(c){if(1==c.h)return a.i?(null==(b=window.navigator)?0:b.onLine)?c.s(3):t(c,Ze(a),3):t(c,Ze(a),3);$e(a);c.h=0})},3E4)}
function Ze(a,b){return a.u?a.u:a.u=new Promise(function(c){var d,e,f,g;return w(function(h){switch(h.h){case 1:return d=window.AbortController?new window.AbortController:void 0,f=null==(e=d)?void 0:e.signal,g=!1,wa(h,2,3),d&&(a.j=a.U.N(function(){d.abort()},b||2E4)),t(h,fetch("/generate_204",{method:"HEAD",
signal:f}),5);case 5:g=!0;case 3:za(h);a.u=void 0;a.j&&(a.U.T(a.j),a.j=0);g!==a.i&&(a.i=g,a.i?We(a,"networkstatus-online"):We(a,"networkstatus-offline"));c(g);Ca(h);break;case 2:ya(h),g=!1,h.s(3)}})})}
;var cf={no:"WEB_DISPLAY_MODE_UNKNOWN",jo:"WEB_DISPLAY_MODE_BROWSER",lo:"WEB_DISPLAY_MODE_MINIMAL_UI",mo:"WEB_DISPLAY_MODE_STANDALONE",ko:"WEB_DISPLAY_MODE_FULLSCREEN"};function df(){this.data_=[];this.h=-1}
df.prototype.set=function(a,b){b=void 0===b?!0:b;0<=a&&52>a&&0===a%1&&this.data_[a]!=b&&(this.data_[a]=b,this.h=-1)};
df.prototype.get=function(a){return!!this.data_[a]};
function ef(a){-1==a.h&&(a.h=ib(a.data_,function(b,c,d){return c?b+Math.pow(2,d):b},0));
return a.h}
;function ff(a,b){this.j=a;this.l=b;this.i=0;this.h=null}
ff.prototype.get=function(){if(0<this.i){this.i--;var a=this.h;this.h=a.next;a.next=null}else a=this.j();return a};
function gf(a,b){a.l(b);100>a.i&&(a.i++,b.next=a.h,a.h=b)}
;var hf;function jf(){var a=x.MessageChannel;"undefined"===typeof a&&"undefined"!==typeof window&&window.postMessage&&window.addEventListener&&!B("Presto")&&(a=function(){var e=Md("IFRAME");e.style.display="none";document.documentElement.appendChild(e);var f=e.contentWindow;e=f.document;e.open();e.close();var g="callImmediate"+Math.random(),h="file:"==f.location.protocol?"*":f.location.protocol+"//"+f.location.host;e=Xa(function(k){if(("*"==h||k.origin==h)&&k.data==g)this.port1.onmessage()},this);
f.addEventListener("message",e,!1);this.port1={};this.port2={postMessage:function(){f.postMessage(g,h)}}});
if("undefined"!==typeof a&&!B("Trident")&&!B("MSIE")){var b=new a,c={},d=c;b.port1.onmessage=function(){if(void 0!==c.next){c=c.next;var e=c.nb;c.nb=null;e()}};
return function(e){d.next={nb:e};d=d.next;b.port2.postMessage(0)}}return function(e){x.setTimeout(e,0)}}
;function kf(){this.i=this.h=null}
kf.prototype.add=function(a,b){var c=lf.get();c.set(a,b);this.i?this.i.next=c:this.h=c;this.i=c};
kf.prototype.remove=function(){var a=null;this.h&&(a=this.h,this.h=this.h.next,this.h||(this.i=null),a.next=null);return a};
var lf=new ff(function(){return new mf},function(a){return a.reset()});
function mf(){this.next=this.scope=this.h=null}
mf.prototype.set=function(a,b){this.h=a;this.scope=b;this.next=null};
mf.prototype.reset=function(){this.next=this.scope=this.h=null};function nf(a,b){of||pf();qf||(of(),qf=!0);rf.add(a,b)}
var of;function pf(){if(x.Promise&&x.Promise.resolve){var a=x.Promise.resolve(void 0);of=function(){a.then(sf)}}else of=function(){var b=sf;
"function"!==typeof x.setImmediate||x.Window&&x.Window.prototype&&!B("Edge")&&x.Window.prototype.setImmediate==x.setImmediate?(hf||(hf=jf()),hf(b)):x.setImmediate(b)}}
var qf=!1,rf=new kf;function sf(){for(var a;a=rf.remove();){try{a.h.call(a.scope)}catch(b){qd(b)}gf(lf,a)}qf=!1}
;function tf(a,b){this.h=a[x.Symbol.iterator]();this.i=b}
tf.prototype[Symbol.iterator]=function(){return this};
tf.prototype.next=function(){var a=this.h.next();return{value:a.done?void 0:this.i.call(void 0,a.value),done:a.done}};
function uf(a,b){return new tf(a,b)}
;function vf(){this.blockSize=-1}
;function wf(){this.blockSize=-1;this.blockSize=64;this.h=[];this.o=[];this.m=[];this.j=[];this.j[0]=128;for(var a=1;a<this.blockSize;++a)this.j[a]=0;this.l=this.i=0;this.reset()}
$a(wf,vf);wf.prototype.reset=function(){this.h[0]=1732584193;this.h[1]=4023233417;this.h[2]=2562383102;this.h[3]=271733878;this.h[4]=3285377520;this.l=this.i=0};
function xf(a,b,c){c||(c=0);var d=a.m;if("string"===typeof b)for(var e=0;16>e;e++)d[e]=b.charCodeAt(c)<<24|b.charCodeAt(c+1)<<16|b.charCodeAt(c+2)<<8|b.charCodeAt(c+3),c+=4;else for(e=0;16>e;e++)d[e]=b[c]<<24|b[c+1]<<16|b[c+2]<<8|b[c+3],c+=4;for(e=16;80>e;e++){var f=d[e-3]^d[e-8]^d[e-14]^d[e-16];d[e]=(f<<1|f>>>31)&4294967295}b=a.h[0];c=a.h[1];var g=a.h[2],h=a.h[3],k=a.h[4];for(e=0;80>e;e++){if(40>e)if(20>e){f=h^c&(g^h);var m=1518500249}else f=c^g^h,m=1859775393;else 60>e?(f=c&g|h&(c|g),m=2400959708):
(f=c^g^h,m=3395469782);f=(b<<5|b>>>27)+f+k+m+d[e]&4294967295;k=h;h=g;g=(c<<30|c>>>2)&4294967295;c=b;b=f}a.h[0]=a.h[0]+b&4294967295;a.h[1]=a.h[1]+c&4294967295;a.h[2]=a.h[2]+g&4294967295;a.h[3]=a.h[3]+h&4294967295;a.h[4]=a.h[4]+k&4294967295}
wf.prototype.update=function(a,b){if(null!=a){void 0===b&&(b=a.length);for(var c=b-this.blockSize,d=0,e=this.o,f=this.i;d<b;){if(0==f)for(;d<=c;)xf(this,a,d),d+=this.blockSize;if("string"===typeof a)for(;d<b;){if(e[f]=a.charCodeAt(d),++f,++d,f==this.blockSize){xf(this,e);f=0;break}}else for(;d<b;)if(e[f]=a[d],++f,++d,f==this.blockSize){xf(this,e);f=0;break}}this.i=f;this.l+=b}};
wf.prototype.digest=function(){var a=[],b=8*this.l;56>this.i?this.update(this.j,56-this.i):this.update(this.j,this.blockSize-(this.i-56));for(var c=this.blockSize-1;56<=c;c--)this.o[c]=b&255,b/=256;xf(this,this.o);for(c=b=0;5>c;c++)for(var d=24;0<=d;d-=8)a[b]=this.h[c]>>d&255,++b;return a};function yf(a){return"string"==typeof a.className?a.className:a.getAttribute&&a.getAttribute("class")||""}
function zf(a,b){"string"==typeof a.className?a.className=b:a.setAttribute&&a.setAttribute("class",b)}
function Af(a,b){a.classList?b=a.classList.contains(b):(a=a.classList?a.classList:yf(a).match(/\S+/g)||[],b=0<=eb(a,b));return b}
function Bf(){var a=document.body;a.classList?a.classList.remove("inverted-hdpi"):Af(a,"inverted-hdpi")&&zf(a,Array.prototype.filter.call(a.classList?a.classList:yf(a).match(/\S+/g)||[],function(b){return"inverted-hdpi"!=b}).join(" "))}
;var Cf="StopIteration"in x?x.StopIteration:{message:"StopIteration",stack:""};function Df(){}
Df.prototype.h=function(){throw Cf;};
Df.prototype.next=function(){return Ef};
var Ef={done:!0,value:void 0};function Ff(a){return{value:a,done:!1}}
function Gf(a){if(a.done)throw Cf;return a.value}
Df.prototype.Z=function(){return this};function Hf(a){if(a instanceof If||a instanceof Jf||a instanceof Kf)return a;if("function"==typeof a.next)return new If(function(){return a});
if("function"==typeof a[Symbol.iterator])return new If(function(){return a[Symbol.iterator]()});
if("function"==typeof a.Z)return new If(function(){return a.Z()});
throw Error("Not an iterator or iterable.");}
function If(a){this.h=a}
If.prototype.Z=function(){return new Jf(this.h())};
If.prototype[Symbol.iterator]=function(){return new Kf(this.h())};
If.prototype.j=function(){return new Kf(this.h())};
function Jf(a){this.i=a}
r(Jf,Df);Jf.prototype.h=function(){var a=this.i.next();if(a.done)throw Cf;return a.value};
Jf.prototype.next=function(){return this.i.next()};
Jf.prototype[Symbol.iterator]=function(){return new Kf(this.i)};
Jf.prototype.j=function(){return new Kf(this.i)};
function Kf(a){If.call(this,function(){return a});
this.i=a}
r(Kf,If);Kf.prototype.next=function(){return this.i.next()};function Lf(a,b){this.i={};this.h=[];this.ia=this.size=0;var c=arguments.length;if(1<c){if(c%2)throw Error("Uneven number of arguments");for(var d=0;d<c;d+=2)this.set(arguments[d],arguments[d+1])}else if(a)if(a instanceof Lf)for(c=a.Ya(),d=0;d<c.length;d++)this.set(c[d],a.get(c[d]));else for(d in a)this.set(d,a[d])}
l=Lf.prototype;l.Ya=function(){Mf(this);return this.h.concat()};
l.has=function(a){return Nf(this.i,a)};
l.equals=function(a,b){if(this===a)return!0;if(this.size!=a.size)return!1;b=b||Of;Mf(this);for(var c,d=0;c=this.h[d];d++)if(!b(this.get(c),a.get(c)))return!1;return!0};
function Of(a,b){return a===b}
l.isEmpty=function(){return 0==this.size};
l.clear=function(){this.i={};this.ia=this.size=this.h.length=0};
l.remove=function(a){return this.delete(a)};
l.delete=function(a){return Nf(this.i,a)?(delete this.i[a],--this.size,this.ia++,this.h.length>2*this.size&&Mf(this),!0):!1};
function Mf(a){if(a.size!=a.h.length){for(var b=0,c=0;b<a.h.length;){var d=a.h[b];Nf(a.i,d)&&(a.h[c++]=d);b++}a.h.length=c}if(a.size!=a.h.length){var e={};for(c=b=0;b<a.h.length;)d=a.h[b],Nf(e,d)||(a.h[c++]=d,e[d]=1),b++;a.h.length=c}}
l.get=function(a,b){return Nf(this.i,a)?this.i[a]:b};
l.set=function(a,b){Nf(this.i,a)||(this.size+=1,this.h.push(a),this.ia++);this.i[a]=b};
l.forEach=function(a,b){for(var c=this.Ya(),d=0;d<c.length;d++){var e=c[d],f=this.get(e);a.call(b,f,e,this)}};
l.clone=function(){return new Lf(this)};
l.keys=function(){return Hf(this.Z(!0)).j()};
l.values=function(){return Hf(this.Z(!1)).j()};
l.entries=function(){var a=this;return uf(this.keys(),function(b){return[b,a.get(b)]})};
l.Z=function(a){Mf(this);var b=0,c=this.ia,d=this,e=new Df;e.next=function(){if(c!=d.ia)throw Error("The map has changed since the iterator was created");if(b>=d.h.length)return Ef;var g=d.h[b++];return Ff(a?g:d.i[g])};
var f=e.next;e.h=function(){return Gf(f.call(e))};
return e};
function Nf(a,b){return Object.prototype.hasOwnProperty.call(a,b)}
;function Pf(a){Qf();return Jb(a)}
var Qf=Ma;function Rf(a){var b=[];Uf(new Vf,a,b);return b.join("")}
function Vf(){}
function Uf(a,b,c){if(null==b)c.push("null");else{if("object"==typeof b){if(Array.isArray(b)){var d=b;b=d.length;c.push("[");for(var e="",f=0;f<b;f++)c.push(e),Uf(a,d[f],c),e=",";c.push("]");return}if(b instanceof String||b instanceof Number||b instanceof Boolean)b=b.valueOf();else{c.push("{");e="";for(d in b)Object.prototype.hasOwnProperty.call(b,d)&&(f=b[d],"function"!=typeof f&&(c.push(e),Wf(d,c),c.push(":"),Uf(a,f,c),e=","));c.push("}");return}}switch(typeof b){case "string":Wf(b,c);break;case "number":c.push(isFinite(b)&&
!isNaN(b)?String(b):"null");break;case "boolean":c.push(String(b));break;case "function":c.push("null");break;default:throw Error("Unknown type: "+typeof b);}}}
var Xf={'"':'\\"',"\\":"\\\\","/":"\\/","\b":"\\b","\f":"\\f","\n":"\\n","\r":"\\r","\t":"\\t","\v":"\\u000b"},Yf=/\uffff/.test("\uffff")?/[\\"\x00-\x1f\x7f-\uffff]/g:/[\\"\x00-\x1f\x7f-\xff]/g;function Wf(a,b){b.push('"',a.replace(Yf,function(c){var d=Xf[c];d||(d="\\u"+(c.charCodeAt(0)|65536).toString(16).substr(1),Xf[c]=d);return d}),'"')}
;function Zf(a){if(!a)return!1;try{return!!a.$goog_Thenable}catch(b){return!1}}
;function $f(a){this.h=0;this.C=void 0;this.l=this.i=this.j=null;this.o=this.m=!1;if(a!=Ma)try{var b=this;a.call(void 0,function(c){ag(b,2,c)},function(c){ag(b,3,c)})}catch(c){ag(this,3,c)}}
function bg(){this.next=this.context=this.onRejected=this.i=this.h=null;this.j=!1}
bg.prototype.reset=function(){this.context=this.onRejected=this.i=this.h=null;this.j=!1};
var cg=new ff(function(){return new bg},function(a){a.reset()});
function dg(a,b,c){var d=cg.get();d.i=a;d.onRejected=b;d.context=c;return d}
function eg(a){return new $f(function(b,c){c(a)})}
$f.prototype.then=function(a,b,c){return fg(this,"function"===typeof a?a:null,"function"===typeof b?b:null,c)};
$f.prototype.$goog_Thenable=!0;function gg(a,b){return fg(a,null,b,void 0)}
$f.prototype.cancel=function(a){if(0==this.h){var b=new hg(a);nf(function(){ig(this,b)},this)}};
function ig(a,b){if(0==a.h)if(a.j){var c=a.j;if(c.i){for(var d=0,e=null,f=null,g=c.i;g&&(g.j||(d++,g.h==a&&(e=g),!(e&&1<d)));g=g.next)e||(f=g);e&&(0==c.h&&1==d?ig(c,b):(f?(d=f,d.next==c.l&&(c.l=d),d.next=d.next.next):jg(c),kg(c,e,3,b)))}a.j=null}else ag(a,3,b)}
function lg(a,b){a.i||2!=a.h&&3!=a.h||mg(a);a.l?a.l.next=b:a.i=b;a.l=b}
function fg(a,b,c,d){var e=dg(null,null,null);e.h=new $f(function(f,g){e.i=b?function(h){try{var k=b.call(d,h);f(k)}catch(m){g(m)}}:f;
e.onRejected=c?function(h){try{var k=c.call(d,h);void 0===k&&h instanceof hg?g(h):f(k)}catch(m){g(m)}}:g});
e.h.j=a;lg(a,e);return e.h}
$f.prototype.A=function(a){this.h=0;ag(this,2,a)};
$f.prototype.L=function(a){this.h=0;ag(this,3,a)};
function ag(a,b,c){if(0==a.h){a===c&&(b=3,c=new TypeError("Promise cannot resolve to itself"));a.h=1;a:{var d=c,e=a.A,f=a.L;if(d instanceof $f){lg(d,dg(e||Ma,f||null,a));var g=!0}else if(Zf(d))d.then(e,f,a),g=!0;else{if(Ra(d))try{var h=d.then;if("function"===typeof h){ng(d,h,e,f,a);g=!0;break a}}catch(k){f.call(a,k);g=!0;break a}g=!1}}g||(a.C=c,a.h=b,a.j=null,mg(a),3!=b||c instanceof hg||og(a,c))}}
function ng(a,b,c,d,e){function f(k){h||(h=!0,d.call(e,k))}
function g(k){h||(h=!0,c.call(e,k))}
var h=!1;try{b.call(a,g,f)}catch(k){f(k)}}
function mg(a){a.m||(a.m=!0,nf(a.u,a))}
function jg(a){var b=null;a.i&&(b=a.i,a.i=b.next,b.next=null);a.i||(a.l=null);return b}
$f.prototype.u=function(){for(var a;a=jg(this);)kg(this,a,this.h,this.C);this.m=!1};
function kg(a,b,c,d){if(3==c&&b.onRejected&&!b.j)for(;a&&a.o;a=a.j)a.o=!1;if(b.h)b.h.j=null,pg(b,c,d);else try{b.j?b.i.call(b.context):pg(b,c,d)}catch(e){qg.call(null,e)}gf(cg,b)}
function pg(a,b,c){2==b?a.i.call(a.context,c):a.onRejected&&a.onRejected.call(a.context,c)}
function og(a,b){a.o=!0;nf(function(){a.o&&qg.call(null,b)})}
var qg=qd;function hg(a){bb.call(this,a)}
$a(hg,bb);hg.prototype.name="cancel";function L(a){K.call(this);this.u=1;this.l=[];this.m=0;this.i=[];this.j={};this.A=!!a}
$a(L,K);l=L.prototype;l.subscribe=function(a,b,c){var d=this.j[a];d||(d=this.j[a]=[]);var e=this.u;this.i[e]=a;this.i[e+1]=b;this.i[e+2]=c;this.u=e+3;d.push(e);return e};
function rg(a,b,c,d){if(b=a.j[b]){var e=a.i;(b=b.find(function(f){return e[f+1]==c&&e[f+2]==d}))&&a.ta(b)}}
l.ta=function(a){var b=this.i[a];if(b){var c=this.j[b];0!=this.m?(this.l.push(a),this.i[a+1]=Ma):(c&&kb(c,a),delete this.i[a],delete this.i[a+1],delete this.i[a+2])}return!!b};
l.ja=function(a,b){var c=this.j[a];if(c){for(var d=Array(arguments.length-1),e=1,f=arguments.length;e<f;e++)d[e-1]=arguments[e];if(this.A)for(e=0;e<c.length;e++){var g=c[e];sg(this.i[g+1],this.i[g+2],d)}else{this.m++;try{for(e=0,f=c.length;e<f&&!this.h();e++)g=c[e],this.i[g+1].apply(this.i[g+2],d)}finally{if(this.m--,0<this.l.length&&0==this.m)for(;c=this.l.pop();)this.ta(c)}}return 0!=e}return!1};
function sg(a,b,c){nf(function(){a.apply(b,c)})}
l.clear=function(a){if(a){var b=this.j[a];b&&(b.forEach(this.ta,this),delete this.j[a])}else this.i.length=0,this.j={}};
l.I=function(){L.aa.I.call(this);this.clear();this.l.length=0};function tg(a){this.h=a}
tg.prototype.set=function(a,b){void 0===b?this.h.remove(a):this.h.set(a,Rf(b))};
tg.prototype.get=function(a){try{var b=this.h.get(a)}catch(c){return}if(null!==b)try{return JSON.parse(b)}catch(c){throw"Storage: Invalid value was encountered";}};
tg.prototype.remove=function(a){this.h.remove(a)};function ug(a){this.h=a}
$a(ug,tg);function vg(a){this.data=a}
function wg(a){return void 0===a||a instanceof vg?a:new vg(a)}
ug.prototype.set=function(a,b){ug.aa.set.call(this,a,wg(b))};
ug.prototype.i=function(a){a=ug.aa.get.call(this,a);if(void 0===a||a instanceof Object)return a;throw"Storage: Invalid value was encountered";};
ug.prototype.get=function(a){if(a=this.i(a)){if(a=a.data,void 0===a)throw"Storage: Invalid value was encountered";}else a=void 0;return a};function xg(a){this.h=a}
$a(xg,ug);xg.prototype.set=function(a,b,c){if(b=wg(b)){if(c){if(c<Date.now()){xg.prototype.remove.call(this,a);return}b.expiration=c}b.creation=Date.now()}xg.aa.set.call(this,a,b)};
xg.prototype.i=function(a){var b=xg.aa.i.call(this,a);if(b){var c=b.creation,d=b.expiration;if(d&&d<Date.now()||c&&c>Date.now())xg.prototype.remove.call(this,a);else return b}};function yg(){}
;function zg(){}
$a(zg,yg);zg.prototype[Symbol.iterator]=function(){return Hf(this.Z(!0)).j()};
zg.prototype.clear=function(){var a=Array.from(this);a=p(a);for(var b=a.next();!b.done;b=a.next())this.remove(b.value)};function Ag(a){this.h=a}
$a(Ag,zg);l=Ag.prototype;l.isAvailable=function(){if(!this.h)return!1;try{return this.h.setItem("__sak","1"),this.h.removeItem("__sak"),!0}catch(a){return!1}};
l.set=function(a,b){try{this.h.setItem(a,b)}catch(c){if(0==this.h.length)throw"Storage mechanism: Storage disabled";throw"Storage mechanism: Quota exceeded";}};
l.get=function(a){a=this.h.getItem(a);if("string"!==typeof a&&null!==a)throw"Storage mechanism: Invalid value was encountered";return a};
l.remove=function(a){this.h.removeItem(a)};
l.Z=function(a){var b=0,c=this.h,d=new Df;d.next=function(){if(b>=c.length)return Ef;var f=c.key(b++);if(a)return Ff(f);f=c.getItem(f);if("string"!==typeof f)throw"Storage mechanism: Invalid value was encountered";return Ff(f)};
var e=d.next;d.h=function(){return Gf(e.call(d))};
return d};
l.clear=function(){this.h.clear()};
l.key=function(a){return this.h.key(a)};function Bg(){var a=null;try{a=window.localStorage||null}catch(b){}this.h=a}
$a(Bg,Ag);function Cg(a,b){this.i=a;this.h=null;var c;if(c=sc)c=!(9<=Number(Fc));if(c){Dg||(Dg=new Lf);this.h=Dg.get(a);this.h||(b?this.h=document.getElementById(b):(this.h=document.createElement("userdata"),this.h.addBehavior("#default#userData"),document.body.appendChild(this.h)),Dg.set(a,this.h));try{this.h.load(this.i)}catch(d){this.h=null}}}
$a(Cg,zg);var Eg={".":".2E","!":".21","~":".7E","*":".2A","'":".27","(":".28",")":".29","%":"."},Dg=null;function Fg(a){return"_"+encodeURIComponent(a).replace(/[.!~*'()%]/g,function(b){return Eg[b]})}
l=Cg.prototype;l.isAvailable=function(){return!!this.h};
l.set=function(a,b){this.h.setAttribute(Fg(a),b);Gg(this)};
l.get=function(a){a=this.h.getAttribute(Fg(a));if("string"!==typeof a&&null!==a)throw"Storage mechanism: Invalid value was encountered";return a};
l.remove=function(a){this.h.removeAttribute(Fg(a));Gg(this)};
l.Z=function(a){var b=0,c=this.h.XMLDocument.documentElement.attributes,d=new Df;d.next=function(){if(b>=c.length)return Ef;var f=c[b++];if(a)return Ff(decodeURIComponent(f.nodeName.replace(/\./g,"%")).substr(1));f=f.nodeValue;if("string"!==typeof f)throw"Storage mechanism: Invalid value was encountered";return Ff(f)};
var e=d.next;d.h=function(){return Gf(e.call(d))};
return d};
l.clear=function(){for(var a=this.h.XMLDocument.documentElement,b=a.attributes.length;0<b;b--)a.removeAttribute(a.attributes[b-1].nodeName);Gg(this)};
function Gg(a){try{a.h.save(a.i)}catch(b){throw"Storage mechanism: Quota exceeded";}}
;function Hg(a,b){this.i=a;this.h=b+"::"}
$a(Hg,zg);Hg.prototype.set=function(a,b){this.i.set(this.h+a,b)};
Hg.prototype.get=function(a){return this.i.get(this.h+a)};
Hg.prototype.remove=function(a){this.i.remove(this.h+a)};
Hg.prototype.Z=function(a){var b=this.i[Symbol.iterator](),c=this,d=new Df;d.next=function(){var f=b.next();if(f.done)return f;for(f=f.value;f.substr(0,c.h.length)!=c.h;){f=b.next();if(f.done)return f;f=f.value}return Ff(a?f.substr(c.h.length):c.i.get(f))};
var e=d.next;d.h=function(){return Gf(e.call(d))};
return d};function Ig(a){J.call(this,a)}
r(Ig,J);Ig.prototype.getKey=function(){return hd(this,1)};
Ig.prototype.getValue=function(){return hd(this,2===kd(this,Jg)?2:-1)};
Ig.prototype.setValue=function(a){return jd(this,2,Jg,a)};
var Jg=[2,3,4,5,6];function Kg(a){J.call(this,a)}
r(Kg,J);function Lg(a){J.call(this,a)}
r(Lg,J);function Mg(a){J.call(this,a)}
r(Mg,J);function Ng(a){J.call(this,a,-1,Og)}
r(Ng,J);Ng.prototype.getPlayerType=function(){return hd(this,36)};
Ng.prototype.setHomeGroupInfo=function(a){return I(this,81,a)};
var Og=[9,66,24,32,86,100,101];function Pg(a){J.call(this,a,-1,Qg)}
r(Pg,J);var Qg=[15,26,28];function Rg(a){J.call(this,a)}
r(Rg,J);Rg.prototype.setToken=function(a){return D(this,2,a)};function Sg(a){J.call(this,a,-1,Tg)}
r(Sg,J);Sg.prototype.setSafetyMode=function(a){return D(this,5,a)};
var Tg=[12];function Ug(a){J.call(this,a,-1,Vg)}
r(Ug,J);var Vg=[12];function Wg(a){J.call(this,a,-1,Xg)}
r(Wg,J);function Yg(a){J.call(this,a)}
r(Yg,J);Yg.prototype.getKey=function(){return pd(this,1)};
Yg.prototype.getValue=function(){return pd(this,2)};
Yg.prototype.setValue=function(a){return D(this,2,a)};
var Xg=[4,5];function Zg(a){J.call(this,a)}
r(Zg,J);function $g(a){J.call(this,a)}
r($g,J);var ah=[2,3];function bh(a){J.call(this,a)}
r(bh,J);bh.prototype.getMessage=function(){return pd(this,1)};function ch(a){J.call(this,a)}
r(ch,J);function dh(a){J.call(this,a)}
r(dh,J);function eh(a){J.call(this,a,-1,fh)}
r(eh,J);var fh=[10,17];function gh(a){J.call(this,a)}
r(gh,J);function hh(a){J.call(this,a)}
r(hh,J);function ih(a){J.call(this,a)}
r(ih,J);var jh={Ah:0,lh:1,rh:2,sh:4,xh:8,th:16,uh:32,zh:64,yh:128,nh:256,ph:512,wh:1024,oh:2048,qh:4096,mh:8192,vh:16384};function kh(a){J.call(this,a)}
r(kh,J);function lh(a,b){I(a,1,b)}
kh.prototype.X=function(a){D(this,2,a)};
function mh(a){J.call(this,a)}
r(mh,J);function nh(a,b){I(a,1,b)}
;function oh(a){J.call(this,a,-1,ph)}
r(oh,J);oh.prototype.X=function(a){D(this,1,a)};
function qh(a,b){I(a,2,b)}
var ph=[3];function rh(a){J.call(this,a)}
r(rh,J);rh.prototype.X=function(a){D(this,1,a)};function sh(a){J.call(this,a)}
r(sh,J);sh.prototype.X=function(a){D(this,1,a)};function th(a){J.call(this,a)}
r(th,J);th.prototype.X=function(a){D(this,1,a)};function uh(a){J.call(this,a)}
r(uh,J);function vh(a){J.call(this,a)}
r(vh,J);function wh(a){J.call(this,a,-1,xh)}
r(wh,J);wh.prototype.getPlayerType=function(){var a=hd(this,7);return null==a?0:a};
wh.prototype.setVideoId=function(a){return D(this,19,a)};
function yh(a,b){od(a,68,zh,b)}
function zh(a){J.call(this,a)}
r(zh,J);zh.prototype.getId=function(){return pd(this,2)};
var xh=[83,68];function Ah(a){J.call(this,a)}
r(Ah,J);function Bh(a){J.call(this,a)}
r(Bh,J);function Ch(a){J.call(this,a)}
r(Ch,J);function Dh(a){J.call(this,a,428)}
r(Dh,J);
var Eh=[23,24,11,6,7,5,2,3,20,21,28,32,37,229,241,45,59,225,288,72,73,78,208,156,202,215,74,76,79,80,111,85,91,97,100,102,105,119,126,127,136,146,157,158,159,163,164,168,176,222,383,177,178,179,411,184,188,189,190,191,193,194,195,196,198,199,200,201,203,204,205,206,258,259,260,261,209,226,227,232,233,234,240,247,248,251,254,255,270,278,291,293,300,304,308,309,310,311,313,314,319,321,323,324,328,330,331,332,337,338,340,344,348,350,351,352,353,354,355,356,357,358,361,363,364,368,369,370,373,374,375,
378,380,381,388,389,403,412,413,414,415,416,417,418,419,420,423,424,425,426,427,117];function Fh(a){J.call(this,a)}
r(Fh,J);function Gh(a){J.call(this,a)}
r(Gh,J);Gh.prototype.setVideoId=function(a){return jd(this,1,Hh,a)};
Gh.prototype.getPlaylistId=function(){return hd(this,2===kd(this,Hh)?2:-1)};
var Hh=[1,2];function Ih(a){J.call(this,a,-1,Jh)}
r(Ih,J);var Jh=[3];function Kh(a){J.call(this,a,1)}
r(Kh,J);function Lh(a){J.call(this,a)}
r(Lh,J);var Mh;Mh=new function(a,b){this.h=a;this.fieldName=b;this.isRepeated=0;this.i=yd}(406606992,{Ho:0},Lh);function Nh(){Lh.apply(this,arguments)}
r(Nh,Lh);oe(Nh);function Oh(a,b){1<b.length?a[b[0]]=b[1]:1===b.length&&Object.assign(a,b[0])}
;var Ph=x.window,Qh,Rh,Sh=(null==Ph?void 0:null==(Qh=Ph.yt)?void 0:Qh.config_)||(null==Ph?void 0:null==(Rh=Ph.ytcfg)?void 0:Rh.data_)||{},Th,Uh=(null==Ph?void 0:null==(Th=Ph.ytcfg)?void 0:Th.obfuscatedData_)||[];function Vh(){Kh.apply(this,arguments)}
r(Vh,Kh);oe(Vh);var Wh=new Vh(Uh),Xh=Sh.EXPERIMENT_FLAGS;if(!Xh||!Xh.jspb_i18n_extension){var Yh=new Nh;Mh.i(Wh,Yh)}y("yt.config_",Sh,void 0);y("yt.configJspb_",Uh,void 0);function Zh(){Oh(Sh,arguments)}
function A(a,b){return a in Sh?Sh[a]:b}
function $h(a){return A(a,void 0)}
function ai(){var a=Sh.EXPERIMENT_FLAGS;return a?a.web_disable_gel_stp_ecatcher_killswitch:void 0}
;function M(a){a=bi(a);return"string"===typeof a&&"false"===a?!1:!!a}
function ci(a,b){a=bi(a);return void 0===a&&void 0!==b?b:Number(a||0)}
function di(){return A("EXPERIMENTS_TOKEN","")}
function bi(a){var b=A("EXPERIMENTS_FORCED_FLAGS",{});return void 0!==b[a]?b[a]:A("EXPERIMENT_FLAGS",{})[a]}
function ei(){var a=[],b=A("EXPERIMENTS_FORCED_FLAGS",{});for(c in b)a.push({key:c,value:String(b[c])});var c=A("EXPERIMENT_FLAGS",{});for(var d in c)d.startsWith("force_")&&void 0===b[d]&&a.push({key:d,value:String(c[d])});return a}
;var fi=[];function gi(a){fi.forEach(function(b){return b(a)})}
function hi(a){return a&&window.yterr?function(){try{return a.apply(this,arguments)}catch(b){ii(b)}}:a}
function ii(a,b,c,d){var e=z("yt.logging.errors.log");e?e(a,"ERROR",b,c,d):(e=A("ERRORS",[]),e.push([a,"ERROR",b,c,d]),Zh("ERRORS",e));gi(a)}
function ji(a,b,c,d){var e=z("yt.logging.errors.log");e?e(a,"WARNING",b,c,d):(e=A("ERRORS",[]),e.push([a,"WARNING",b,c,d]),Zh("ERRORS",e))}
;function ki(){var a=li;z("yt.ads.biscotti.getId_")||y("yt.ads.biscotti.getId_",a,void 0)}
function mi(a){y("yt.ads.biscotti.lastId_",a,void 0)}
;var ni=/^[\w.]*$/,oi={q:!0,search_query:!0};function pi(a,b){b=a.split(b);for(var c={},d=0,e=b.length;d<e;d++){var f=b[d].split("=");if(1==f.length&&f[0]||2==f.length)try{var g=qi(f[0]||""),h=qi(f[1]||"");g in c?Array.isArray(c[g])?lb(c[g],h):c[g]=[c[g],h]:c[g]=h}catch(u){var k=u,m=f[0],q=String(pi);k.args=[{key:m,value:f[1],query:a,method:ri==q?"unchanged":q}];oi.hasOwnProperty(m)||ji(k)}}return c}
var ri=String(pi);function si(a){var b=[];mb(a,function(c,d){var e=encodeURIComponent(String(d)),f;Array.isArray(c)?f=c:f=[c];fb(f,function(g){""==g?b.push(e):b.push(e+"="+encodeURIComponent(String(g)))})});
return b.join("&")}
function ti(a){"?"==a.charAt(0)&&(a=a.substr(1));return pi(a,"&")}
function ui(a){return-1!=a.indexOf("?")?(a=(a||"").split("#")[0],a=a.split("?",2),ti(1<a.length?a[1]:a[0])):{}}
function vi(a,b,c){var d=a.split("#",2);a=d[0];d=1<d.length?"#"+d[1]:"";var e=a.split("?",2);a=e[0];e=ti(e[1]||"");for(var f in b)!c&&null!==e&&f in e||(e[f]=b[f]);return hc(a,e)+d}
function wi(a){if(!b)var b=window.location.href;var c=cc(1,a),d=dc(a);c&&d?(a=a.match(ac),b=b.match(ac),a=a[3]==b[3]&&a[1]==b[1]&&a[4]==b[4]):a=d?dc(b)==d&&(Number(cc(4,b))||null)==(Number(cc(4,a))||null):!0;return a}
function qi(a){return a&&a.match(ni)?a:decodeURIComponent(a.replace(/\+/g," "))}
;function xi(a){var b=yi;a=void 0===a?z("yt.ads.biscotti.lastId_")||"":a;var c=Object,d=c.assign,e={};e.dt=Yd;e.flash="0";a:{try{var f=b.h.top.location.href}catch(sa){f=2;break a}f=f?f===b.i.location.href?0:1:2}e=(e.frm=f,e);try{e.u_tz=-(new Date).getTimezoneOffset();var g=void 0===g?Cd:g;try{var h=g.history.length}catch(sa){h=0}e.u_his=h;var k;e.u_h=null==(k=Cd.screen)?void 0:k.height;var m;e.u_w=null==(m=Cd.screen)?void 0:m.width;var q;e.u_ah=null==(q=Cd.screen)?void 0:q.availHeight;var u;e.u_aw=
null==(u=Cd.screen)?void 0:u.availWidth;var C;e.u_cd=null==(C=Cd.screen)?void 0:C.colorDepth}catch(sa){}h=b.h;try{var v=h.screenX;var E=h.screenY}catch(sa){}try{var F=h.outerWidth;var H=h.outerHeight}catch(sa){}try{var R=h.innerWidth;var O=h.innerHeight}catch(sa){}try{var S=h.screenLeft;var ia=h.screenTop}catch(sa){}try{R=h.innerWidth,O=h.innerHeight}catch(sa){}try{var N=h.screen.availWidth;var Aa=h.screen.availTop}catch(sa){}v=[S,ia,v,E,N,Aa,F,H,R,O];try{var Na=(b.h.top||window).document,ta="CSS1Compat"==
Na.compatMode?Na.documentElement:Na.body;var G=(new Kd(ta.clientWidth,ta.clientHeight)).round()}catch(sa){G=new Kd(-12245933,-12245933)}Na=G;G={};var Ba=void 0===Ba?x:Ba;ta=new df;Ba.SVGElement&&Ba.document.createElementNS&&ta.set(0);E=Sd();E["allow-top-navigation-by-user-activation"]&&ta.set(1);E["allow-popups-to-escape-sandbox"]&&ta.set(2);Ba.crypto&&Ba.crypto.subtle&&ta.set(3);Ba.TextDecoder&&Ba.TextEncoder&&ta.set(4);Ba=ef(ta);G.bc=Ba;G.bih=Na.height;G.biw=Na.width;G.brdim=v.join();b=b.i;b=(G.vis=
b.prerendering?3:{visible:1,hidden:2,prerender:3,preview:4,unloaded:5}[b.visibilityState||b.webkitVisibilityState||b.mozVisibilityState||""]||0,G.wgl=!!Cd.WebGLRenderingContext,G);c=d.call(c,e,b);c.ca_type="image";a&&(c.bid=a);return c}
var yi=new function(){var a=window.document;this.h=window;this.i=a};
y("yt.ads_.signals_.getAdSignalsString",function(a){return si(xi(a))},void 0);Date.now();var zi="XMLHttpRequest"in x?function(){return new XMLHttpRequest}:null;
function Ai(){if(!zi)return null;var a=zi();return"open"in a?a:null}
function Bi(a){switch(a&&"status"in a?a.status:-1){case 200:case 201:case 202:case 203:case 204:case 205:case 206:case 304:return!0;default:return!1}}
;function Ci(a,b){"function"===typeof a&&(a=hi(a));return window.setTimeout(a,b)}
function Di(a){window.clearTimeout(a)}
;var Ei={Authorization:"AUTHORIZATION","X-Goog-Visitor-Id":"SANDBOXED_VISITOR_ID","X-Youtube-Domain-Admin-State":"DOMAIN_ADMIN_STATE","X-Youtube-Chrome-Connected":"CHROME_CONNECTED_HEADER","X-YouTube-Client-Name":"INNERTUBE_CONTEXT_CLIENT_NAME","X-YouTube-Client-Version":"INNERTUBE_CONTEXT_CLIENT_VERSION","X-YouTube-Delegation-Context":"INNERTUBE_CONTEXT_SERIALIZED_DELEGATION_CONTEXT","X-YouTube-Device":"DEVICE","X-Youtube-Identity-Token":"ID_TOKEN","X-YouTube-Page-CL":"PAGE_CL","X-YouTube-Page-Label":"PAGE_BUILD_LABEL",
"X-YouTube-Variants-Checksum":"VARIANTS_CHECKSUM"},Fi="app debugcss debugjs expflag force_ad_params force_ad_encrypted force_viral_ad_response_params forced_experiments innertube_snapshots innertube_goldens internalcountrycode internalipoverride absolute_experiments conditional_experiments sbb sr_bns_address".split(" ").concat(fa("client_dev_mss_url client_dev_regex_map client_dev_root_url client_rollout_override expflag jsfeat jsmode mods".split(" "))),aj=!1;
function bj(a,b){b=void 0===b?{}:b;var c=wi(a),d=M("web_ajax_ignore_global_headers_if_set"),e;for(e in Ei){var f=A(Ei[e]);!f||!c&&dc(a)||d&&void 0!==b[e]||(b[e]=f)}if(c||!dc(a))b["X-YouTube-Utc-Offset"]=String(-(new Date).getTimezoneOffset());if(c||!dc(a)){try{var g=(new Intl.DateTimeFormat).resolvedOptions().timeZone}catch(h){}g&&(b["X-YouTube-Time-Zone"]=g)}if(c||!dc(a))b["X-YouTube-Ad-Signals"]=si(xi(void 0));return b}
function cj(a){var b=window.location.search,c=dc(a);M("debug_handle_relative_url_for_query_forward_killswitch")||c||!wi(a)||(c=document.location.hostname);var d=bc(cc(5,a));d=(c=c&&(c.endsWith("youtube.com")||c.endsWith("youtube-nocookie.com")))&&d&&d.startsWith("/api/");if(!c||d)return a;var e=ti(b),f={};fb(Fi,function(g){e[g]&&(f[g]=e[g])});
return vi(a,f||{},!1)}
function dj(a,b){var c=b.format||"JSON";a=ej(a,b);var d=fj(a,b),e=!1,f=gj(a,function(k){if(!e){e=!0;h&&Di(h);var m=Bi(k),q=null,u=400<=k.status&&500>k.status,C=500<=k.status&&600>k.status;if(m||u||C)q=hj(a,c,k,b.convertToSafeHtml);if(m)a:if(k&&204==k.status)m=!0;else{switch(c){case "XML":m=0==parseInt(q&&q.return_code,10);break a;case "RAW":m=!0;break a}m=!!q}q=q||{};u=b.context||x;m?b.onSuccess&&b.onSuccess.call(u,k,q):b.onError&&b.onError.call(u,k,q);b.onFinish&&b.onFinish.call(u,k,q)}},b.method,
d,b.headers,b.responseType,b.withCredentials);
if(b.onTimeout&&0<b.timeout){var g=b.onTimeout;var h=Ci(function(){e||(e=!0,f.abort(),Di(h),g.call(b.context||x,f))},b.timeout)}return f}
function ej(a,b){b.includeDomain&&(a=document.location.protocol+"//"+document.location.hostname+(document.location.port?":"+document.location.port:"")+a);var c=A("XSRF_FIELD_NAME",void 0);if(b=b.urlParams)b[c]&&delete b[c],a=vi(a,b||{},!0);return a}
function fj(a,b){var c=A("XSRF_FIELD_NAME",void 0),d=A("XSRF_TOKEN",void 0),e=b.postBody||"",f=b.postParams,g=$h("XSRF_FIELD_NAME"),h;b.headers&&(h=b.headers["Content-Type"]);b.excludeXsrf||dc(a)&&!b.withCredentials&&dc(a)!=document.location.hostname||"POST"!=b.method||h&&"application/x-www-form-urlencoded"!=h||b.postParams&&b.postParams[g]||(f||(f={}),f[c]=d);f&&"string"===typeof e&&(e=ti(e),yb(e,f),e=b.postBodyFormat&&"JSON"==b.postBodyFormat?JSON.stringify(e):gc(e));f=e||f&&!pb(f);!aj&&f&&"POST"!=
b.method&&(aj=!0,ii(Error("AJAX request with postData should use POST")));return e}
function hj(a,b,c,d){var e=null;switch(b){case "JSON":try{var f=c.responseText}catch(g){throw d=Error("Error reading responseText"),d.params=a,ji(d),g;}a=c.getResponseHeader("Content-Type")||"";f&&0<=a.indexOf("json")&&(")]}'\n"===f.substring(0,5)&&(f=f.substring(5)),e=JSON.parse(f));break;case "XML":if(a=(a=c.responseXML)?ij(a):null)e={},fb(a.getElementsByTagName("*"),function(g){e[g.tagName]=jj(g)})}d&&kj(e);
return e}
function kj(a){if(Ra(a))for(var b in a){var c;(c="html_content"==b)||(c=b.length-5,c=0<=c&&b.indexOf("_html",c)==c);if(c){c=b;Db("HTML that is escaped and sanitized server-side and passed through yt.net.ajax");var d=a[b],e=Ab();d=e?e.createHTML(d):d;a[c]=new Ub(d)}else kj(a[b])}}
function ij(a){return a?(a=("responseXML"in a?a.responseXML:a).getElementsByTagName("root"))&&0<a.length?a[0]:null:null}
function jj(a){var b="";fb(a.childNodes,function(c){b+=c.nodeValue});
return b}
function gj(a,b,c,d,e,f,g){function h(){4==(k&&"readyState"in k?k.readyState:0)&&b&&hi(b)(k)}
c=void 0===c?"GET":c;d=void 0===d?"":d;var k=Ai();if(!k)return null;"onloadend"in k?k.addEventListener("loadend",h,!1):k.onreadystatechange=h;M("debug_forward_web_query_parameters")&&(a=cj(a));k.open(c,a,!0);f&&(k.responseType=f);g&&(k.withCredentials=!0);c="POST"==c&&(void 0===window.FormData||!(d instanceof FormData));if(e=bj(a,e))for(var m in e)k.setRequestHeader(m,e[m]),"content-type"==m.toLowerCase()&&(c=!1);c&&k.setRequestHeader("Content-Type","application/x-www-form-urlencoded");k.send(d);
return k}
;function lj(a){var b=this;this.h=void 0;a.addEventListener("beforeinstallprompt",function(c){c.preventDefault();b.h=c})}
function mj(){if(!x.matchMedia)return"WEB_DISPLAY_MODE_UNKNOWN";try{return x.matchMedia("(display-mode: standalone)").matches?"WEB_DISPLAY_MODE_STANDALONE":x.matchMedia("(display-mode: minimal-ui)").matches?"WEB_DISPLAY_MODE_MINIMAL_UI":x.matchMedia("(display-mode: fullscreen)").matches?"WEB_DISPLAY_MODE_FULLSCREEN":x.matchMedia("(display-mode: browser)").matches?"WEB_DISPLAY_MODE_BROWSER":"WEB_DISPLAY_MODE_UNKNOWN"}catch(a){return"WEB_DISPLAY_MODE_UNKNOWN"}}
function nj(){var a=mj();a=Object.keys(cf).indexOf(a);return-1===a?null:a}
;function oj(a,b,c,d,e){je.set(""+a,b,{Oa:c,path:"/",domain:void 0===d?"youtube.com":d,secure:void 0===e?!1:e})}
function pj(a){return je.get(""+a,void 0)}
function qj(){if(!je.isEnabled())return!1;if(!je.isEmpty())return!0;je.set("TESTCOOKIESENABLED","1",{Oa:60});if("1"!==je.get("TESTCOOKIESENABLED"))return!1;je.remove("TESTCOOKIESENABLED");return!0}
;var rj=z("ytglobal.prefsUserPrefsPrefs_")||{};y("ytglobal.prefsUserPrefsPrefs_",rj,void 0);function sj(){this.h=A("ALT_PREF_COOKIE_NAME","PREF");this.i=A("ALT_PREF_COOKIE_DOMAIN","youtube.com");var a=pj(this.h);if(a){a=decodeURIComponent(a).split("&");for(var b=0;b<a.length;b++){var c=a[b].split("="),d=c[0];(c=c[1])&&(rj[d]=c.toString())}}}
sj.prototype.get=function(a,b){tj(a);uj(a);a=void 0!==rj[a]?rj[a].toString():null;return null!=a?a:b?b:""};
sj.prototype.set=function(a,b){tj(a);uj(a);if(null==b)throw Error("ExpectedNotNull");rj[a]=b.toString()};
function vj(a){return!!((wj("f"+(Math.floor(a/31)+1))||0)&1<<a%31)}
sj.prototype.remove=function(a){tj(a);uj(a);delete rj[a]};
sj.prototype.clear=function(){for(var a in rj)delete rj[a]};
function uj(a){if(/^f([1-9][0-9]*)$/.test(a))throw Error("ExpectedRegexMatch: "+a);}
function tj(a){if(!/^\w+$/.test(a))throw Error("ExpectedRegexMismatch: "+a);}
function wj(a){a=void 0!==rj[a]?rj[a].toString():null;return null!=a&&/^[A-Fa-f0-9]+$/.test(a)?parseInt(a,16):null}
Oa(sj);var xj={bluetooth:"CONN_DISCO",cellular:"CONN_CELLULAR_UNKNOWN",ethernet:"CONN_WIFI",none:"CONN_NONE",wifi:"CONN_WIFI",wimax:"CONN_CELLULAR_4G",other:"CONN_UNKNOWN",unknown:"CONN_UNKNOWN","slow-2g":"CONN_CELLULAR_2G","2g":"CONN_CELLULAR_2G","3g":"CONN_CELLULAR_3G","4g":"CONN_CELLULAR_4G"},yj={CONN_DEFAULT:0,CONN_UNKNOWN:1,CONN_NONE:2,CONN_WIFI:3,CONN_CELLULAR_2G:4,CONN_CELLULAR_3G:5,CONN_CELLULAR_4G:6,CONN_CELLULAR_UNKNOWN:7,CONN_DISCO:8,CONN_CELLULAR_5G:9,CONN_WIFI_METERED:10,CONN_CELLULAR_5G_SA:11,
CONN_CELLULAR_5G_NSA:12,CONN_INVALID:31},zj={EFFECTIVE_CONNECTION_TYPE_UNKNOWN:0,EFFECTIVE_CONNECTION_TYPE_OFFLINE:1,EFFECTIVE_CONNECTION_TYPE_SLOW_2G:2,EFFECTIVE_CONNECTION_TYPE_2G:3,EFFECTIVE_CONNECTION_TYPE_3G:4,EFFECTIVE_CONNECTION_TYPE_4G:5},Aj={"slow-2g":"EFFECTIVE_CONNECTION_TYPE_SLOW_2G","2g":"EFFECTIVE_CONNECTION_TYPE_2G","3g":"EFFECTIVE_CONNECTION_TYPE_3G","4g":"EFFECTIVE_CONNECTION_TYPE_4G"};function Bj(){var a=x.navigator;return a?a.connection:void 0}
function Cj(){var a=Bj();if(a){var b=xj[a.type||"unknown"]||"CONN_UNKNOWN";a=xj[a.effectiveType||"unknown"]||"CONN_UNKNOWN";"CONN_CELLULAR_UNKNOWN"===b&&"CONN_UNKNOWN"!==a&&(b=a);if("CONN_UNKNOWN"!==b)return b;if("CONN_UNKNOWN"!==a)return a}}
function Dj(){var a=Bj();if(null!=a&&a.effectiveType)return Aj.hasOwnProperty(a.effectiveType)?Aj[a.effectiveType]:"EFFECTIVE_CONNECTION_TYPE_UNKNOWN"}
;function Ej(){return"INNERTUBE_API_KEY"in Sh&&"INNERTUBE_API_VERSION"in Sh}
function Fj(){return{innertubeApiKey:A("INNERTUBE_API_KEY",void 0),innertubeApiVersion:A("INNERTUBE_API_VERSION",void 0),ab:A("INNERTUBE_CONTEXT_CLIENT_CONFIG_INFO"),bb:A("INNERTUBE_CONTEXT_CLIENT_NAME","WEB"),Wb:A("INNERTUBE_CONTEXT_CLIENT_NAME",1),innertubeContextClientVersion:A("INNERTUBE_CONTEXT_CLIENT_VERSION",void 0),xb:A("INNERTUBE_CONTEXT_HL",void 0),wb:A("INNERTUBE_CONTEXT_GL",void 0),Xb:A("INNERTUBE_HOST_OVERRIDE",void 0)||"",Zb:!!A("INNERTUBE_USE_THIRD_PARTY_AUTH",!1),Yb:!!A("INNERTUBE_OMIT_API_KEY_WHEN_AUTH_HEADER_IS_PRESENT",
!1),appInstallData:A("SERIALIZED_CLIENT_CONFIG_DATA",void 0)}}
function Gj(a){var b={client:{hl:a.xb,gl:a.wb,clientName:a.bb,clientVersion:a.innertubeContextClientVersion,configInfo:a.ab}};navigator.userAgent&&(b.client.userAgent=String(navigator.userAgent));var c=x.devicePixelRatio;c&&1!=c&&(b.client.screenDensityFloat=String(c));c=di();""!==c&&(b.client.experimentsToken=c);c=ei();0<c.length&&(b.request={internalExperimentFlags:c});Hj(a,void 0,b);Ij(a,void 0,b);Jj(void 0,b);Kj(a,void 0,b);Lj(void 0,b);A("DELEGATED_SESSION_ID")&&!M("pageid_as_header_web")&&(b.user=
{onBehalfOfUser:A("DELEGATED_SESSION_ID")});a=Object;c=a.assign;for(var d=b.client,e={},f=p(Object.entries(ti(A("DEVICE","")))),g=f.next();!g.done;g=f.next()){var h=p(g.value);g=h.next().value;h=h.next().value;"cbrand"===g?e.deviceMake=h:"cmodel"===g?e.deviceModel=h:"cbr"===g?e.browserName=h:"cbrver"===g?e.browserVersion=h:"cos"===g?e.osName=h:"cosver"===g?e.osVersion=h:"cplatform"===g&&(e.platform=h)}b.client=c.call(a,d,e);return b}
function Mj(a){var b=new Ug,c=new Ng;D(c,1,a.xb);D(c,2,a.wb);D(c,16,a.Wb);D(c,17,a.innertubeContextClientVersion);if(a.ab){var d=a.ab,e=new Kg;d.coldConfigData&&D(e,1,d.coldConfigData);d.appInstallData&&D(e,6,d.appInstallData);d.coldHashData&&D(e,3,d.coldHashData);d.hotHashData&&D(e,5,d.hotHashData);I(c,62,e)}(d=x.devicePixelRatio)&&1!=d&&D(c,65,d);d=di();""!==d&&D(c,54,d);d=ei();if(0<d.length){e=new Pg;for(var f=0;f<d.length;f++){var g=new Ig;D(g,1,d[f].key);g.setValue(d[f].value);od(e,15,Ig,g)}I(b,
5,e)}Hj(a,c);Ij(a,c);Jj(c);Kj(a,c);Lj(c);A("DELEGATED_SESSION_ID")&&!M("pageid_as_header_web")&&(a=new Sg,D(a,3,A("DELEGATED_SESSION_ID")));a=p(Object.entries(ti(A("DEVICE",""))));for(d=a.next();!d.done;d=a.next())e=p(d.value),d=e.next().value,e=e.next().value,"cbrand"===d?D(c,12,e):"cmodel"===d?D(c,13,e):"cbr"===d?D(c,87,e):"cbrver"===d?D(c,88,e):"cos"===d?D(c,18,e):"cosver"===d?D(c,19,e):"cplatform"===d&&D(c,42,e);I(b,1,c);return b}
function Hj(a,b,c){a=a.bb;if("WEB"===a||"MWEB"===a||1===a||2===a)if(b){c=ld(b,Lg,96)||new Lg;var d=nj();null!==d&&D(c,3,d);I(b,96,c)}else c&&(c.client.mainAppWebInfo=null!=(d=c.client.mainAppWebInfo)?d:{},c.client.mainAppWebInfo.webDisplayMode=mj())}
function Ij(a,b,c){a=a.bb;if(("WEB_REMIX"===a||76===a)&&!M("music_web_display_mode_killswitch"))if(b){var d;c=null!=(d=ld(b,Mg,70))?d:new Mg;d=nj();null!==d&&D(c,10,d);I(b,70,c)}else if(c){var e;c.client.Ab=null!=(e=c.client.Ab)?e:{};c.client.Ab.webDisplayMode=mj()}}
function Jj(a,b){var c;if(M("web_log_memory_total_kbytes")&&(null==(c=x.navigator)?0:c.deviceMemory)){var d;c=null==(d=x.navigator)?void 0:d.deviceMemory;a?D(a,95,1E6*c):b&&(b.client.memoryTotalKbytes=""+1E6*c)}}
function Kj(a,b,c){if(a.appInstallData)if(b){var d;c=null!=(d=ld(b,Kg,62))?d:new Kg;D(c,6,a.appInstallData);I(b,62,c)}else c&&(c.client.configInfo=c.client.configInfo||{},c.client.configInfo.appInstallData=a.appInstallData)}
function Lj(a,b){var c=Cj();c&&(a?D(a,61,yj[c]):b&&(b.client.connectionType=c));M("web_log_effective_connection_type")&&(c=Dj())&&(a?D(a,94,zj[c]):b&&(b.client.effectiveConnectionType=c))}
function Nj(a,b,c){c=void 0===c?{}:c;var d={};M("enable_web_eom_visitor_data")&&A("EOM_VISITOR_DATA")?d={"X-Goog-EOM-Visitor-Id":A("EOM_VISITOR_DATA")}:d={"X-Goog-Visitor-Id":c.visitorData||A("VISITOR_DATA","")};if(b&&b.includes("www.youtube-nocookie.com"))return d;(b=c.Co||A("AUTHORIZATION"))||(a?b="Bearer "+z("gapi.auth.getToken")().Bo:b=ne([]));b&&(d.Authorization=b,d["X-Goog-AuthUser"]=A("SESSION_INDEX",0),M("pageid_as_header_web")&&(d["X-Goog-PageId"]=A("DELEGATED_SESSION_ID")));return d}
;function Oj(a){a=Object.assign({},a);delete a.Authorization;var b=ne();if(b){var c=new wf;c.update(A("INNERTUBE_API_KEY",void 0));c.update(b);a.hash=Lc(c.digest(),3)}return a}
;function Pj(a){var b=new Bg;(b=b.isAvailable()?a?new Hg(b,a):b:null)||(a=new Cg(a||"UserDataSharedStore"),b=a.isAvailable()?a:null);this.h=(a=b)?new xg(a):null;this.i=document.domain||window.location.hostname}
Pj.prototype.set=function(a,b,c,d){c=c||31104E3;this.remove(a);if(this.h)try{this.h.set(a,b,Date.now()+1E3*c);return}catch(f){}var e="";if(d)try{e=escape(Rf(b))}catch(f){return}else e=escape(b);oj(a,e,c,this.i)};
Pj.prototype.get=function(a,b){var c=void 0,d=!this.h;if(!d)try{c=this.h.get(a)}catch(e){d=!0}if(d&&(c=pj(a))&&(c=unescape(c),b))try{c=JSON.parse(c)}catch(e){this.remove(a),c=void 0}return c};
Pj.prototype.remove=function(a){this.h&&this.h.remove(a);var b=this.i;je.remove(""+a,"/",void 0===b?"youtube.com":b)};var Qj=window,P=Qj.ytcsi&&Qj.ytcsi.now?Qj.ytcsi.now:Qj.performance&&Qj.performance.timing&&Qj.performance.now&&Qj.performance.timing.navigationStart?function(){return Qj.performance.timing.navigationStart+Qj.performance.now()}:function(){return(new Date).getTime()};var Rj;function Sj(){Rj||(Rj=new Pj("yt.innertube"));return Rj}
function Tj(a,b,c,d){if(d)return null;d=Sj().get("nextId",!0)||1;var e=Sj().get("requests",!0)||{};e[d]={method:a,request:b,authState:Oj(c),requestTime:Math.round(P())};Sj().set("nextId",d+1,86400,!0);Sj().set("requests",e,86400,!0);return d}
function Uj(a){var b=Sj().get("requests",!0)||{};delete b[a];Sj().set("requests",b,86400,!0)}
function Vj(a){var b=Sj().get("requests",!0);if(b){for(var c in b){var d=b[c];if(!(6E4>Math.round(P())-d.requestTime)){var e=d.authState,f=Oj(Nj(!1));ub(e,f)&&(e=d.request,"requestTimeMs"in e&&(e.requestTimeMs=Math.round(P())),Wj(a,d.method,e,{}));delete b[c]}}Sj().set("requests",b,86400,!0)}}
;function Xj(){}
function Yj(a,b){return Zj(a,0,b)}
Xj.prototype.N=function(a,b){return Zj(a,1,b)};
function ak(a,b){Zj(a,2,b)}
function bk(a){var b=z("yt.scheduler.instance.addImmediateJob");b?b(a):a()}
;function ck(){Xj.apply(this,arguments)}
r(ck,Xj);function dk(){ck.h||(ck.h=new ck);return ck.h}
function Zj(a,b,c){void 0!==c&&Number.isNaN(Number(c))&&(c=void 0);var d=z("yt.scheduler.instance.addJob");return d?d(a,b,c):void 0===c?(a(),NaN):Ci(a,c||0)}
ck.prototype.T=function(a){if(void 0===a||!Number.isNaN(Number(a))){var b=z("yt.scheduler.instance.cancelJob");b?b(a):Di(a)}};
ck.prototype.start=function(){var a=z("yt.scheduler.instance.start");a&&a()};
ck.prototype.pause=function(){var a=z("yt.scheduler.instance.pause");a&&a()};var bf=dk();var ek=Gc||Hc;function fk(a){var b=Rb();return b?0<=b.toLowerCase().indexOf(a):!1}
;var gk=function(){var a;return function(){a||(a=new Pj("ytidb"));return a}}();
function hk(){var a;return null==(a=gk())?void 0:a.get("LAST_RESULT_ENTRY_KEY",!0)}
;var ik=[],jk,kk=!1;function lk(){var a={};for(jk=new mk(void 0===a.handleError?nk:a.handleError,void 0===a.logEvent?ok:a.logEvent);0<ik.length;)switch(a=ik.shift(),a.type){case "ERROR":jk.handleError(a.payload);break;case "EVENT":jk.logEvent(a.eventType,a.payload)}}
function pk(a){kk||(jk?jk.handleError(a):(ik.push({type:"ERROR",payload:a}),10<ik.length&&ik.shift()))}
function qk(a,b){kk||(jk?jk.logEvent(a,b):(ik.push({type:"EVENT",eventType:a,payload:b}),10<ik.length&&ik.shift()))}
;function Q(a){var b=Ja.apply(1,arguments);var c=Error.call(this,a);this.message=c.message;"stack"in c&&(this.stack=c.stack);this.args=[].concat(fa(b))}
r(Q,Error);function rk(){try{return sk(),!0}catch(a){return!1}}
function sk(a){if(void 0!==A("DATASYNC_ID",void 0))return A("DATASYNC_ID",void 0);throw new Q("Datasync ID not set",void 0===a?"unknown":a);}
;function tk(a){if(0<=a.indexOf(":"))throw Error("Database name cannot contain ':'");}
function uk(a){return a.substr(0,a.indexOf(":"))||a}
;var vk={},wk=(vk.AUTH_INVALID="No user identifier specified.",vk.EXPLICIT_ABORT="Transaction was explicitly aborted.",vk.IDB_NOT_SUPPORTED="IndexedDB is not supported.",vk.MISSING_INDEX="Index not created.",vk.MISSING_OBJECT_STORES="Object stores not created.",vk.DB_DELETED_BY_MISSING_OBJECT_STORES="Database is deleted because expected object stores were not created.",vk.DB_REOPENED_BY_MISSING_OBJECT_STORES="Database is reopened because expected object stores were not created.",vk.UNKNOWN_ABORT="Transaction was aborted for unknown reasons.",
vk.QUOTA_EXCEEDED="The current transaction exceeded its quota limitations.",vk.QUOTA_MAYBE_EXCEEDED="The current transaction may have failed because of exceeding quota limitations.",vk.EXECUTE_TRANSACTION_ON_CLOSED_DB="Can't start a transaction on a closed database",vk.INCOMPATIBLE_DB_VERSION="The binary is incompatible with the database version",vk),xk={},yk=(xk.AUTH_INVALID="ERROR",xk.EXECUTE_TRANSACTION_ON_CLOSED_DB="WARNING",xk.EXPLICIT_ABORT="IGNORED",xk.IDB_NOT_SUPPORTED="ERROR",xk.MISSING_INDEX=
"WARNING",xk.MISSING_OBJECT_STORES="ERROR",xk.DB_DELETED_BY_MISSING_OBJECT_STORES="WARNING",xk.DB_REOPENED_BY_MISSING_OBJECT_STORES="WARNING",xk.QUOTA_EXCEEDED="WARNING",xk.QUOTA_MAYBE_EXCEEDED="WARNING",xk.UNKNOWN_ABORT="WARNING",xk.INCOMPATIBLE_DB_VERSION="WARNING",xk),zk={},Ak=(zk.AUTH_INVALID=!1,zk.EXECUTE_TRANSACTION_ON_CLOSED_DB=!1,zk.EXPLICIT_ABORT=!1,zk.IDB_NOT_SUPPORTED=!1,zk.MISSING_INDEX=!1,zk.MISSING_OBJECT_STORES=!1,zk.DB_DELETED_BY_MISSING_OBJECT_STORES=!1,zk.DB_REOPENED_BY_MISSING_OBJECT_STORES=
!1,zk.QUOTA_EXCEEDED=!1,zk.QUOTA_MAYBE_EXCEEDED=!0,zk.UNKNOWN_ABORT=!0,zk.INCOMPATIBLE_DB_VERSION=!1,zk);function Bk(a,b,c,d,e){b=void 0===b?{}:b;c=void 0===c?wk[a]:c;d=void 0===d?yk[a]:d;e=void 0===e?Ak[a]:e;Q.call(this,c,Object.assign({},{name:"YtIdbKnownError",isSw:void 0===self.document,isIframe:self!==self.top,type:a},b));this.type=a;this.message=c;this.level=d;this.h=e;Object.setPrototypeOf(this,Bk.prototype)}
r(Bk,Q);function Ck(a,b){Bk.call(this,"MISSING_OBJECT_STORES",{expectedObjectStores:b,foundObjectStores:a},wk.MISSING_OBJECT_STORES);Object.setPrototypeOf(this,Ck.prototype)}
r(Ck,Bk);function Dk(a,b){var c=Error.call(this);this.message=c.message;"stack"in c&&(this.stack=c.stack);this.index=a;this.objectStore=b;Object.setPrototypeOf(this,Dk.prototype)}
r(Dk,Error);var Ek=["The database connection is closing","Can't start a transaction on a closed database","A mutation operation was attempted on a database that did not allow mutations"];
function Fk(a,b,c,d){b=uk(b);var e=a instanceof Error?a:Error("Unexpected error: "+a);if(e instanceof Bk)return e;a={objectStoreNames:c,dbName:b,dbVersion:d};if("QuotaExceededError"===e.name)return new Bk("QUOTA_EXCEEDED",a);if(Ic&&"UnknownError"===e.name)return new Bk("QUOTA_MAYBE_EXCEEDED",a);if(e instanceof Dk)return new Bk("MISSING_INDEX",Object.assign({},a,{objectStore:e.objectStore,index:e.index}));if("InvalidStateError"===e.name&&Ek.some(function(f){return e.message.includes(f)}))return new Bk("EXECUTE_TRANSACTION_ON_CLOSED_DB",
a);
if("AbortError"===e.name)return new Bk("UNKNOWN_ABORT",a,e.message);e.args=[Object.assign({},a,{name:"IdbError",Cb:e.name})];e.level="WARNING";return e}
function Gk(a,b,c){var d=hk();return new Bk("IDB_NOT_SUPPORTED",{context:{caller:a,publicName:b,version:c,hasSucceededOnce:null==d?void 0:d.hasSucceededOnce}})}
;function Hk(a){if(!a)throw Error();throw a;}
function Ik(a){return a}
function Jk(a){this.h=a}
function Kk(a){function b(e){if("PENDING"===d.state.status){d.state={status:"REJECTED",reason:e};e=p(d.onRejected);for(var f=e.next();!f.done;f=e.next())f=f.value,f()}}
function c(e){if("PENDING"===d.state.status){d.state={status:"FULFILLED",value:e};e=p(d.h);for(var f=e.next();!f.done;f=e.next())f=f.value,f()}}
var d=this;this.state={status:"PENDING"};this.h=[];this.onRejected=[];a=a.h;try{a(c,b)}catch(e){b(e)}}
Kk.all=function(a){return new Kk(new Jk(function(b,c){var d=[],e=a.length;0===e&&b(d);for(var f={ma:0};f.ma<a.length;f={ma:f.ma},++f.ma)Lk(Kk.resolve(a[f.ma]).then(function(g){return function(h){d[g.ma]=h;e--;0===e&&b(d)}}(f)),function(g){c(g)})}))};
Kk.resolve=function(a){return new Kk(new Jk(function(b,c){a instanceof Kk?a.then(b,c):b(a)}))};
Kk.reject=function(a){return new Kk(new Jk(function(b,c){c(a)}))};
Kk.prototype.then=function(a,b){var c=this,d=null!=a?a:Ik,e=null!=b?b:Hk;return new Kk(new Jk(function(f,g){"PENDING"===c.state.status?(c.h.push(function(){Mk(c,c,d,f,g)}),c.onRejected.push(function(){Nk(c,c,e,f,g)})):"FULFILLED"===c.state.status?Mk(c,c,d,f,g):"REJECTED"===c.state.status&&Nk(c,c,e,f,g)}))};
function Lk(a,b){a.then(void 0,b)}
function Mk(a,b,c,d,e){try{if("FULFILLED"!==a.state.status)throw Error("calling handleResolve before the promise is fulfilled.");var f=c(a.state.value);f instanceof Kk?Ok(a,b,f,d,e):d(f)}catch(g){e(g)}}
function Nk(a,b,c,d,e){try{if("REJECTED"!==a.state.status)throw Error("calling handleReject before the promise is rejected.");var f=c(a.state.reason);f instanceof Kk?Ok(a,b,f,d,e):d(f)}catch(g){e(g)}}
function Ok(a,b,c,d,e){b===c?e(new TypeError("Circular promise chain detected.")):c.then(function(f){f instanceof Kk?Ok(a,b,f,d,e):d(f)},function(f){e(f)})}
;function Pk(a,b,c){function d(){c(a.error);f()}
function e(){b(a.result);f()}
function f(){try{a.removeEventListener("success",e),a.removeEventListener("error",d)}catch(g){}}
a.addEventListener("success",e);a.addEventListener("error",d)}
function Qk(a){return new Promise(function(b,c){Pk(a,b,c)})}
function Rk(a){return new Kk(new Jk(function(b,c){Pk(a,b,c)}))}
;function Sk(a,b){return new Kk(new Jk(function(c,d){function e(){var f=a?b(a):null;f?f.then(function(g){a=g;e()},d):c()}
e()}))}
;function Tk(a,b){this.h=a;this.options=b;this.transactionCount=0;this.j=Math.round(P());this.i=!1}
l=Tk.prototype;l.add=function(a,b,c){return Uk(this,[a],{mode:"readwrite",O:!0},function(d){return d.objectStore(a).add(b,c)})};
l.clear=function(a){return Uk(this,[a],{mode:"readwrite",O:!0},function(b){return b.objectStore(a).clear()})};
l.close=function(){this.h.close();var a;(null==(a=this.options)?0:a.closed)&&this.options.closed()};
l.count=function(a,b){return Uk(this,[a],{mode:"readonly",O:!0},function(c){return c.objectStore(a).count(b)})};
function Vk(a,b,c){a=a.h.createObjectStore(b,c);return new Wk(a)}
l.delete=function(a,b){return Uk(this,[a],{mode:"readwrite",O:!0},function(c){return c.objectStore(a).delete(b)})};
l.get=function(a,b){return Uk(this,[a],{mode:"readonly",O:!0},function(c){return c.objectStore(a).get(b)})};
function Xk(a,b){return Uk(a,["LogsRequestsStore"],{mode:"readwrite",O:!0},function(c){c=c.objectStore("LogsRequestsStore");return Rk(c.h.put(b,void 0))})}
l.objectStoreNames=function(){return Array.from(this.h.objectStoreNames)};
function Uk(a,b,c,d){var e,f,g,h,k,m,q,u,C,v,E,F;return w(function(H){switch(H.h){case 1:var R={mode:"readonly",O:!1,tag:"IDB_TRANSACTION_TAG_UNKNOWN"};"string"===typeof c?R.mode=c:Object.assign(R,c);e=R;a.transactionCount++;f=e.O?3:1;g=0;case 2:if(h){H.s(3);break}g++;k=Math.round(P());wa(H,4);m=a.h.transaction(b,e.mode);R=new Yk(m);R=Zk(R,d);return t(H,R,6);case 6:return q=H.i,u=Math.round(P()),$k(a,k,u,g,void 0,b.join(),e),H.return(q);case 4:C=ya(H);v=Math.round(P());E=Fk(C,a.h.name,b.join(),a.h.version);
if((F=E instanceof Bk&&!E.h)||g>=f)$k(a,k,v,g,E,b.join(),e),h=E;H.s(2);break;case 3:return H.return(Promise.reject(h))}})}
function $k(a,b,c,d,e,f,g){b=c-b;e?(e instanceof Bk&&("QUOTA_EXCEEDED"===e.type||"QUOTA_MAYBE_EXCEEDED"===e.type)&&qk("QUOTA_EXCEEDED",{dbName:uk(a.h.name),objectStoreNames:f,transactionCount:a.transactionCount,transactionMode:g.mode}),e instanceof Bk&&"UNKNOWN_ABORT"===e.type&&(c-=a.j,0>c&&c>=Math.pow(2,31)&&(c=0),qk("TRANSACTION_UNEXPECTEDLY_ABORTED",{objectStoreNames:f,transactionDuration:b,transactionCount:a.transactionCount,dbDuration:c}),a.i=!0),al(a,!1,d,f,b,g.tag),pk(e)):al(a,!0,d,f,b,g.tag)}
function al(a,b,c,d,e,f){qk("TRANSACTION_ENDED",{objectStoreNames:d,connectionHasUnknownAbortedTransaction:a.i,duration:e,isSuccessful:b,tryCount:c,tag:void 0===f?"IDB_TRANSACTION_TAG_UNKNOWN":f})}
l.getName=function(){return this.h.name};
function Wk(a){this.h=a}
l=Wk.prototype;l.add=function(a,b){return Rk(this.h.add(a,b))};
l.autoIncrement=function(){return this.h.autoIncrement};
l.clear=function(){return Rk(this.h.clear()).then(function(){})};
l.count=function(a){return Rk(this.h.count(a))};
function bl(a,b){return cl(a,{query:b},function(c){return c.delete().then(function(){return c.continue()})}).then(function(){})}
l.delete=function(a){return a instanceof IDBKeyRange?bl(this,a):Rk(this.h.delete(a))};
l.get=function(a){return Rk(this.h.get(a))};
l.index=function(a){try{return new dl(this.h.index(a))}catch(b){if(b instanceof Error&&"NotFoundError"===b.name)throw new Dk(a,this.h.name);throw b;}};
l.getName=function(){return this.h.name};
l.keyPath=function(){return this.h.keyPath};
function cl(a,b,c){a=a.h.openCursor(b.query,b.direction);return el(a).then(function(d){return Sk(d,c)})}
function Yk(a){var b=this;this.h=a;this.j=new Map;this.i=!1;this.done=new Promise(function(c,d){b.h.addEventListener("complete",function(){c()});
b.h.addEventListener("error",function(e){e.currentTarget===e.target&&d(b.h.error)});
b.h.addEventListener("abort",function(){var e=b.h.error;if(e)d(e);else if(!b.i){e=Bk;for(var f=b.h.objectStoreNames,g=[],h=0;h<f.length;h++){var k=f.item(h);if(null===k)throw Error("Invariant: item in DOMStringList is null");g.push(k)}e=new e("UNKNOWN_ABORT",{objectStoreNames:g.join(),dbName:b.h.db.name,mode:b.h.mode});d(e)}})})}
function Zk(a,b){var c=new Promise(function(d,e){try{Lk(b(a).then(function(f){d(f)}),e)}catch(f){e(f),a.abort()}});
return Promise.all([c,a.done]).then(function(d){return p(d).next().value})}
Yk.prototype.abort=function(){this.h.abort();this.i=!0;throw new Bk("EXPLICIT_ABORT");};
Yk.prototype.objectStore=function(a){a=this.h.objectStore(a);var b=this.j.get(a);b||(b=new Wk(a),this.j.set(a,b));return b};
function dl(a){this.h=a}
l=dl.prototype;l.count=function(a){return Rk(this.h.count(a))};
l.delete=function(a){return fl(this,{query:a},function(b){return b.delete().then(function(){return b.continue()})})};
l.get=function(a){return Rk(this.h.get(a))};
l.getKey=function(a){return Rk(this.h.getKey(a))};
l.keyPath=function(){return this.h.keyPath};
l.unique=function(){return this.h.unique};
function fl(a,b,c){a=a.h.openCursor(void 0===b.query?null:b.query,void 0===b.direction?"next":b.direction);return el(a).then(function(d){return Sk(d,c)})}
function gl(a,b){this.request=a;this.cursor=b}
function el(a){return Rk(a).then(function(b){return b?new gl(a,b):null})}
l=gl.prototype;l.advance=function(a){this.cursor.advance(a);return el(this.request)};
l.continue=function(a){this.cursor.continue(a);return el(this.request)};
l.delete=function(){return Rk(this.cursor.delete()).then(function(){})};
l.getKey=function(){return this.cursor.key};
l.getValue=function(){return this.cursor.value};
l.update=function(a){return Rk(this.cursor.update(a))};function hl(a,b,c){return new Promise(function(d,e){function f(){C||(C=new Tk(g.result,{closed:u}));return C}
var g=void 0!==b?self.indexedDB.open(a,b):self.indexedDB.open(a);var h=c.blocked,k=c.blocking,m=c.Ac,q=c.upgrade,u=c.closed,C;g.addEventListener("upgradeneeded",function(v){try{if(null===v.newVersion)throw Error("Invariant: newVersion on IDbVersionChangeEvent is null");if(null===g.transaction)throw Error("Invariant: transaction on IDbOpenDbRequest is null");v.dataLoss&&"none"!==v.dataLoss&&qk("IDB_DATA_CORRUPTED",{reason:v.dataLossMessage||"unknown reason",dbName:uk(a)});var E=f(),F=new Yk(g.transaction);
q&&q(E,function(H){return v.oldVersion<H&&v.newVersion>=H},F);
F.done.catch(function(H){e(H)})}catch(H){e(H)}});
g.addEventListener("success",function(){var v=g.result;k&&v.addEventListener("versionchange",function(){k(f())});
v.addEventListener("close",function(){qk("IDB_UNEXPECTEDLY_CLOSED",{dbName:uk(a),dbVersion:v.version});m&&m()});
d(f())});
g.addEventListener("error",function(){e(g.error)});
h&&g.addEventListener("blocked",function(){h()})})}
function il(a,b,c){c=void 0===c?{}:c;return hl(a,b,c)}
function jl(a,b){b=void 0===b?{}:b;var c,d,e,f;return w(function(g){if(1==g.h)return wa(g,2),c=self.indexedDB.deleteDatabase(a),d=b,(e=d.blocked)&&c.addEventListener("blocked",function(){e()}),t(g,Qk(c),4);
if(2!=g.h)return xa(g,0);f=ya(g);throw Fk(f,a,"",-1);})}
;function kl(a){return new Promise(function(b){ak(function(){b()},a)})}
function ll(a,b){this.name=a;this.options=b;this.l=!0;this.m=this.o=0;this.i=500}
ll.prototype.j=function(a,b,c){c=void 0===c?{}:c;return il(a,b,c)};
ll.prototype.delete=function(a){a=void 0===a?{}:a;return jl(this.name,a)};
function ml(a,b){return new Bk("INCOMPATIBLE_DB_VERSION",{dbName:a.name,oldVersion:a.options.version,newVersion:b})}
function nl(a,b){if(!b)throw Gk("openWithToken",uk(a.name));return a.open()}
ll.prototype.open=function(){function a(){var f,g,h,k,m,q,u,C,v,E;return w(function(F){switch(F.h){case 1:return g=null!=(f=Error().stack)?f:"",wa(F,2),t(F,c.j(c.name,c.options.version,e),4);case 4:h=F.i;for(var H=c.options,R=[],O=p(Object.keys(H.xa)),S=O.next();!S.done;S=O.next()){S=S.value;var ia=H.xa[S],N=void 0===ia.jc?Number.MAX_VALUE:ia.jc;!(h.h.version>=ia.Va)||h.h.version>=N||h.h.objectStoreNames.contains(S)||R.push(S)}k=R;if(0===k.length){F.s(5);break}m=Object.keys(c.options.xa);q=h.objectStoreNames();
if(c.m<ci("ytidb_reopen_db_retries",0))return c.m++,h.close(),pk(new Bk("DB_REOPENED_BY_MISSING_OBJECT_STORES",{dbName:c.name,expectedObjectStores:m,foundObjectStores:q})),F.return(a());if(!(c.o<ci("ytidb_remake_db_retries",1))){F.s(6);break}c.o++;if(!M("ytidb_remake_db_enable_backoff_delay")){F.s(7);break}return t(F,kl(c.i),8);case 8:c.i*=2;case 7:return t(F,c.delete(),9);case 9:return pk(new Bk("DB_DELETED_BY_MISSING_OBJECT_STORES",{dbName:c.name,expectedObjectStores:m,foundObjectStores:q})),F.return(a());
case 6:throw new Ck(q,m);case 5:return F.return(h);case 2:u=ya(F);if(u instanceof DOMException?"VersionError"!==u.name:"DOMError"in self&&u instanceof DOMError?"VersionError"!==u.name:!(u instanceof Object&&"message"in u)||"An attempt was made to open a database using a lower version than the existing version."!==u.message){F.s(10);break}return t(F,c.j(c.name,void 0,Object.assign({},e,{upgrade:void 0})),11);case 11:C=F.i;v=C.h.version;if(void 0!==c.options.version&&v>c.options.version+1)throw C.close(),
c.l=!1,ml(c,v);return F.return(C);case 10:throw b(),u instanceof Error&&!M("ytidb_async_stack_killswitch")&&(u.stack=u.stack+"\n"+g.substring(g.indexOf("\n")+1)),Fk(u,c.name,"",null!=(E=c.options.version)?E:-1);}})}
function b(){c.h===d&&(c.h=void 0)}
var c=this;if(!this.l)throw ml(this);if(this.h)return this.h;var d,e={blocking:function(f){f.close()},
closed:b,Ac:b,upgrade:this.options.upgrade};return this.h=d=a()};var ol=new ll("YtIdbMeta",{xa:{databases:{Va:1}},upgrade:function(a,b){b(1)&&Vk(a,"databases",{keyPath:"actualName"})}});
function pl(a,b){var c;return w(function(d){if(1==d.h)return t(d,nl(ol,b),2);c=d.i;return d.return(Uk(c,["databases"],{O:!0,mode:"readwrite"},function(e){var f=e.objectStore("databases");return f.get(a.actualName).then(function(g){if(g?a.actualName!==g.actualName||a.publicName!==g.publicName||a.userIdentifier!==g.userIdentifier:1)return Rk(f.h.put(a,void 0)).then(function(){})})}))})}
function ql(a,b){var c;return w(function(d){if(1==d.h)return a?t(d,nl(ol,b),2):d.return();c=d.i;return d.return(c.delete("databases",a))})}
function rl(a,b){var c,d;return w(function(e){return 1==e.h?(c=[],t(e,nl(ol,b),2)):3!=e.h?(d=e.i,t(e,Uk(d,["databases"],{O:!0,mode:"readonly"},function(f){c.length=0;return cl(f.objectStore("databases"),{},function(g){a(g.getValue())&&c.push(g.getValue());return g.continue()})}),3)):e.return(c)})}
function sl(a){return rl(function(b){return"LogsDatabaseV2"===b.publicName&&void 0!==b.userIdentifier},a)}
function tl(a,b,c){return rl(function(d){return c?void 0!==d.userIdentifier&&!a.includes(d.userIdentifier)&&c.includes(d.publicName):void 0!==d.userIdentifier&&!a.includes(d.userIdentifier)},b)}
function ul(a){var b,c;return w(function(d){if(1==d.h)return b=sk("YtIdbMeta hasAnyMeta other"),t(d,rl(function(e){return void 0!==e.userIdentifier&&e.userIdentifier!==b},a),2);
c=d.i;return d.return(0<c.length)})}
;var vl,wl=new function(){}(new function(){});
function xl(){var a,b,c,d;return w(function(e){switch(e.h){case 1:a=hk();if(null==(b=a)?0:b.hasSucceededOnce)return e.return(!0);var f;if(f=ek)f=/WebKit\/([0-9]+)/.exec(Rb()),f=!!(f&&600<=parseInt(f[1],10));f&&(f=/WebKit\/([0-9]+)/.exec(Rb()),f=!(f&&602<=parseInt(f[1],10)));if(f||tc)return e.return(!1);try{if(c=self,!(c.indexedDB&&c.IDBIndex&&c.IDBKeyRange&&c.IDBObjectStore))return e.return(!1)}catch(g){return e.return(!1)}if(!("IDBTransaction"in self&&"objectStoreNames"in IDBTransaction.prototype))return e.return(!1);
wa(e,2);d={actualName:"yt-idb-test-do-not-use",publicName:"yt-idb-test-do-not-use",userIdentifier:void 0};return t(e,pl(d,wl),4);case 4:return t(e,ql("yt-idb-test-do-not-use",wl),5);case 5:return e.return(!0);case 2:return ya(e),e.return(!1)}})}
function yl(){if(void 0!==vl)return vl;kk=!0;return vl=xl().then(function(a){kk=!1;var b;if(null!=(b=gk())&&b.h){var c;b={hasSucceededOnce:(null==(c=hk())?void 0:c.hasSucceededOnce)||a};var d;null==(d=gk())||d.set("LAST_RESULT_ENTRY_KEY",b,2592E3,!0)}return a})}
function zl(){return z("ytglobal.idbToken_")||void 0}
function Al(){var a=zl();return a?Promise.resolve(a):yl().then(function(b){(b=b?wl:void 0)&&y("ytglobal.idbToken_",b,void 0);return b})}
;var Bl=0;function Cl(a,b){Bl||(Bl=bf.N(function(){var c,d,e,f,g;return w(function(h){switch(h.h){case 1:return t(h,Al(),2);case 2:c=h.i;if(!c)return h.return();d=!0;wa(h,3);return t(h,tl(a,c,b),5);case 5:e=h.i;if(!e.length){d=!1;h.s(6);break}f=e[0];return t(h,jl(f.actualName),7);case 7:return t(h,ql(f.actualName,c),6);case 6:xa(h,4);break;case 3:g=ya(h),pk(g),d=!1;case 4:bf.T(Bl),Bl=0,d&&Cl(a,b),h.h=0}})}))}
function Dl(){var a;return w(function(b){return 1==b.h?t(b,Al(),2):(a=b.i)?b.return(ul(a)):b.return(!1)})}
new Ad;function El(a){if(!rk())throw a=new Bk("AUTH_INVALID",{dbName:a}),pk(a),a;var b=sk();return{actualName:a+":"+b,publicName:a,userIdentifier:b}}
function Fl(a,b,c,d){var e,f,g,h,k,m;return w(function(q){switch(q.h){case 1:return f=null!=(e=Error().stack)?e:"",t(q,Al(),2);case 2:g=q.i;if(!g)throw h=Gk("openDbImpl",a,b),M("ytidb_async_stack_killswitch")||(h.stack=h.stack+"\n"+f.substring(f.indexOf("\n")+1)),pk(h),h;tk(a);k=c?{actualName:a,publicName:a,userIdentifier:void 0}:El(a);wa(q,3);return t(q,pl(k,g),5);case 5:return t(q,il(k.actualName,b,d),6);case 6:return q.return(q.i);case 3:return m=ya(q),wa(q,7),t(q,ql(k.actualName,g),9);case 9:xa(q,
8);break;case 7:ya(q);case 8:throw m;}})}
function Gl(a,b,c){c=void 0===c?{}:c;return Fl(a,b,!1,c)}
function Hl(a,b,c){c=void 0===c?{}:c;return Fl(a,b,!0,c)}
function Il(a,b){b=void 0===b?{}:b;var c,d;return w(function(e){if(1==e.h)return t(e,Al(),2);if(3!=e.h){c=e.i;if(!c)return e.return();tk(a);d=El(a);return t(e,jl(d.actualName,b),3)}return t(e,ql(d.actualName,c),0)})}
function Jl(a,b,c){a=a.map(function(d){return w(function(e){return 1==e.h?t(e,jl(d.actualName,b),2):t(e,ql(d.actualName,c),0)})});
return Promise.all(a).then(function(){})}
function Kl(){var a=void 0===a?{}:a;var b,c;return w(function(d){if(1==d.h)return t(d,Al(),2);if(3!=d.h){b=d.i;if(!b)return d.return();tk("LogsDatabaseV2");return t(d,sl(b),3)}c=d.i;return t(d,Jl(c,a,b),0)})}
function Ll(a,b){b=void 0===b?{}:b;var c;return w(function(d){if(1==d.h)return t(d,Al(),2);if(3!=d.h){c=d.i;if(!c)return d.return();tk(a);return t(d,jl(a,b),3)}return t(d,ql(a,c),0)})}
;function Ml(a){this.h=!1;this.potentialEsfErrorCounter=this.i=0;this.handleError=function(){};
this.qa=function(){};
this.now=Date.now;this.wa=!1;var b;this.Jb=null!=(b=a.Jb)?b:100;var c;this.Hb=null!=(c=a.Hb)?c:1;var d;this.Fb=null!=(d=a.Fb)?d:2592E6;var e;this.Db=null!=(e=a.Db)?e:12E4;var f;this.Gb=null!=(f=a.Gb)?f:5E3;var g;this.v=null!=(g=a.v)?g:void 0;this.Ka=!!a.Ka;var h;this.Ja=null!=(h=a.Ja)?h:.1;var k;this.Qa=null!=(k=a.Qa)?k:10;a.handleError&&(this.handleError=a.handleError);a.qa&&(this.qa=a.qa);a.wa&&(this.wa=a.wa);this.D=a.D;this.U=a.U;this.K=a.K;this.J=a.J;this.da=a.da;this.hb=a.hb;this.gb=a.gb;this.v&&
(!this.D||this.D("networkless_logging"))&&Nl(this)}
function Nl(a){a.v&&!a.wa&&(a.h=!0,a.Ka&&Math.random()<=a.Ja&&a.K.Qb(a.v),Ol(a),a.J.H()&&a.Aa(),a.J.V(a.hb,a.Aa.bind(a)),a.J.V(a.gb,a.mb.bind(a)))}
l=Ml.prototype;l.writeThenSend=function(a,b){var c=this;b=void 0===b?{}:b;if(this.v&&this.h){var d={url:a,options:b,timestamp:this.now(),status:"NEW",sendCount:0};this.K.set(d,this.v).then(function(e){d.id=e;c.J.H()&&Pl(c,d)}).catch(function(e){Pl(c,d);
Ql(c,e)})}else this.da(a,b)};
l.sendThenWrite=function(a,b,c){var d=this;b=void 0===b?{}:b;if(this.v&&this.h){var e={url:a,options:b,timestamp:this.now(),status:"NEW",sendCount:0};this.D&&this.D("nwl_skip_retry")&&(e.skipRetry=c);if(this.J.H()||this.D&&this.D("nwl_aggressive_send_then_write")&&!e.skipRetry){if(!e.skipRetry){var f=b.onError?b.onError:function(){};
b.onError=function(g,h){return w(function(k){if(1==k.h)return t(k,d.K.set(e,d.v).catch(function(m){Ql(d,m)}),2);
f(g,h);k.h=0})}}this.da(a,b,e.skipRetry)}else this.K.set(e,this.v).catch(function(g){d.da(a,b,e.skipRetry);
Ql(d,g)})}else this.da(a,b,this.D&&this.D("nwl_skip_retry")&&c)};
l.sendAndWrite=function(a,b){var c=this;b=void 0===b?{}:b;if(this.v&&this.h){var d={url:a,options:b,timestamp:this.now(),status:"NEW",sendCount:0},e=!1,f=b.onSuccess?b.onSuccess:function(){};
d.options.onSuccess=function(g,h){void 0!==d.id?c.K.oa(d.id,c.v):e=!0;c.J.ca&&c.D&&c.D("vss_network_hint")&&c.J.ca(!0);f(g,h)};
this.da(d.url,d.options);this.K.set(d,this.v).then(function(g){d.id=g;e&&c.K.oa(d.id,c.v)}).catch(function(g){Ql(c,g)})}else this.da(a,b)};
l.Aa=function(){var a=this;if(!this.v)throw Gk("throttleSend");this.i||(this.i=this.U.N(function(){var b;return w(function(c){if(1==c.h)return t(c,a.K.vb("NEW",a.v),2);if(3!=c.h)return b=c.i,b?t(c,Pl(a,b),3):(a.mb(),c.return());a.i&&(a.i=0,a.Aa());c.h=0})},this.Jb))};
l.mb=function(){this.U.T(this.i);this.i=0};
function Pl(a,b){var c,d;return w(function(e){switch(e.h){case 1:if(!a.v)throw c=Gk("immediateSend"),c;if(void 0===b.id){e.s(2);break}return t(e,a.K.ac(b.id,a.v),3);case 3:(d=e.i)?b=d:a.qa(Error("The request cannot be found in the database."));case 2:if(Rl(a,b,a.Fb)){e.s(4);break}a.qa(Error("Networkless Logging: Stored logs request expired age limit"));if(void 0===b.id){e.s(5);break}return t(e,a.K.oa(b.id,a.v),5);case 5:return e.return();case 4:b.skipRetry||(b=Sl(a,b));if(!b){e.s(0);break}if(!b.skipRetry||
void 0===b.id){e.s(8);break}return t(e,a.K.oa(b.id,a.v),8);case 8:a.da(b.url,b.options,!!b.skipRetry),e.h=0}})}
function Sl(a,b){if(!a.v)throw Gk("updateRequestHandlers");var c=b.options.onError?b.options.onError:function(){};
b.options.onError=function(e,f){var g,h,k;return w(function(m){switch(m.h){case 1:g=Tl(f);if(!(a.D&&a.D("nwl_consider_error_code")&&g||a.D&&!a.D("nwl_consider_error_code")&&a.potentialEsfErrorCounter<=a.Qa)){m.s(2);break}if(!a.J.ea){m.s(3);break}return t(m,a.J.ea(),3);case 3:if(a.J.H()){m.s(2);break}c(e,f);if(!a.D||!a.D("nwl_consider_error_code")||void 0===(null==(h=b)?void 0:h.id)){m.s(6);break}return t(m,a.K.ib(b.id,a.v,!1),6);case 6:return m.return();case 2:if(a.D&&a.D("nwl_consider_error_code")&&
!g&&a.potentialEsfErrorCounter>a.Qa)return m.return();a.potentialEsfErrorCounter++;if(void 0===(null==(k=b)?void 0:k.id)){m.s(8);break}return b.sendCount<a.Hb?t(m,a.K.ib(b.id,a.v),12):t(m,a.K.oa(b.id,a.v),8);case 12:a.U.N(function(){a.J.H()&&a.Aa()},a.Gb);
case 8:c(e,f),m.h=0}})};
var d=b.options.onSuccess?b.options.onSuccess:function(){};
b.options.onSuccess=function(e,f){var g;return w(function(h){if(1==h.h)return void 0===(null==(g=b)?void 0:g.id)?h.s(2):t(h,a.K.oa(b.id,a.v),2);a.J.ca&&a.D&&a.D("vss_network_hint")&&a.J.ca(!0);d(e,f);h.h=0})};
return b}
function Rl(a,b,c){b=b.timestamp;return a.now()-b>=c?!1:!0}
function Ol(a){if(!a.v)throw Gk("retryQueuedRequests");a.K.vb("QUEUED",a.v).then(function(b){b&&!Rl(a,b,a.Db)?a.U.N(function(){return w(function(c){if(1==c.h)return void 0===b.id?c.s(2):t(c,a.K.ib(b.id,a.v),2);Ol(a);c.h=0})}):a.J.H()&&a.Aa()})}
function Ql(a,b){a.Lb&&!a.J.H()?a.Lb(b):a.handleError(b)}
function Tl(a){var b;return(a=null==a?void 0:null==(b=a.error)?void 0:b.code)&&400<=a&&599>=a?!1:!0}
;function Ul(a,b){this.version=a;this.args=b}
;function Vl(a,b){this.topic=a;this.h=b}
Vl.prototype.toString=function(){return this.topic};var Wl=z("ytPubsub2Pubsub2Instance")||new L;L.prototype.subscribe=L.prototype.subscribe;L.prototype.unsubscribeByKey=L.prototype.ta;L.prototype.publish=L.prototype.ja;L.prototype.clear=L.prototype.clear;y("ytPubsub2Pubsub2Instance",Wl,void 0);var Xl=z("ytPubsub2Pubsub2SubscribedKeys")||{};y("ytPubsub2Pubsub2SubscribedKeys",Xl,void 0);var Yl=z("ytPubsub2Pubsub2TopicToKeys")||{};y("ytPubsub2Pubsub2TopicToKeys",Yl,void 0);var Zl=z("ytPubsub2Pubsub2IsAsync")||{};y("ytPubsub2Pubsub2IsAsync",Zl,void 0);
y("ytPubsub2Pubsub2SkipSubKey",null,void 0);function $l(a,b){var c=am();c&&c.publish.call(c,a.toString(),a,b)}
function bm(a){var b=cm,c=am();if(!c)return 0;var d=c.subscribe(b.toString(),function(e,f){var g=z("ytPubsub2Pubsub2SkipSubKey");g&&g==d||(g=function(){if(Xl[d])try{if(f&&b instanceof Vl&&b!=e)try{var h=b.h,k=f;if(!k.args||!k.version)throw Error("yt.pubsub2.Data.deserialize(): serializedData is incomplete.");try{if(!h.ia){var m=new h;h.ia=m.version}var q=h.ia}catch(H){}if(!q||k.version!=q)throw Error("yt.pubsub2.Data.deserialize(): serializedData version is incompatible.");try{q=Reflect;var u=q.construct;
var C=k.args,v=C.length;if(0<v){var E=Array(v);for(k=0;k<v;k++)E[k]=C[k];var F=E}else F=[];f=u.call(q,h,F)}catch(H){throw H.message="yt.pubsub2.Data.deserialize(): "+H.message,H;}}catch(H){throw H.message="yt.pubsub2.pubsub2 cross-binary conversion error for "+b.toString()+": "+H.message,H;}a.call(window,f)}catch(H){ii(H)}},Zl[b.toString()]?z("yt.scheduler.instance")?bf.N(g):Ci(g,0):g())});
Xl[d]=!0;Yl[b.toString()]||(Yl[b.toString()]=[]);Yl[b.toString()].push(d);return d}
function dm(){var a=em,b=bm(function(c){a.apply(void 0,arguments);fm(b)});
return b}
function fm(a){var b=am();b&&("number"===typeof a&&(a=[a]),fb(a,function(c){b.unsubscribeByKey(c);delete Xl[c]}))}
function am(){return z("ytPubsub2Pubsub2Instance")}
;function gm(a,b){ll.call(this,a,b);this.options=b;tk(a)}
r(gm,ll);function hm(a,b){var c;return function(){c||(c=new gm(a,b));return c}}
gm.prototype.j=function(a,b,c){c=void 0===c?{}:c;return(this.options.jb?Hl:Gl)(a,b,Object.assign({},c))};
gm.prototype.delete=function(a){a=void 0===a?{}:a;return(this.options.jb?Ll:Il)(this.name,a)};
function im(a,b){return hm(a,b)}
;var jm;
function km(){if(jm)return jm();var a={};jm=im("LogsDatabaseV2",{xa:(a.LogsRequestsStore={Va:2},a),jb:!1,upgrade:function(b,c,d){c(2)&&Vk(b,"LogsRequestsStore",{keyPath:"id",autoIncrement:!0});c(3);c(5)&&(d=d.objectStore("LogsRequestsStore"),d.h.indexNames.contains("newRequest")&&d.h.deleteIndex("newRequest"),d.h.createIndex("newRequestV2",["status","interface","timestamp"],{unique:!1}));c(7)&&b.h.objectStoreNames.contains("sapisid")&&b.h.deleteObjectStore("sapisid");c(9)&&b.h.objectStoreNames.contains("SWHealthLog")&&b.h.deleteObjectStore("SWHealthLog")},
version:9});return jm()}
;function lm(a){return nl(km(),a)}
function mm(a,b){var c,d,e,f;return w(function(g){if(1==g.h)return c={startTime:P(),transactionType:"YT_IDB_TRANSACTION_TYPE_WRITE"},t(g,lm(b),2);if(3!=g.h)return d=g.i,e=Object.assign({},a,{options:JSON.parse(JSON.stringify(a.options)),interface:A("INNERTUBE_CONTEXT_CLIENT_NAME",0)}),t(g,Xk(d,e),3);f=g.i;c.Bc=P();nm(c);return g.return(f)})}
function om(a,b){var c,d,e,f,g,h,k;return w(function(m){if(1==m.h)return c={startTime:P(),transactionType:"YT_IDB_TRANSACTION_TYPE_READ"},t(m,lm(b),2);if(3!=m.h)return d=m.i,e=A("INNERTUBE_CONTEXT_CLIENT_NAME",0),f=[a,e,0],g=[a,e,P()],h=IDBKeyRange.bound(f,g),k=void 0,t(m,Uk(d,["LogsRequestsStore"],{mode:"readwrite",O:!0},function(q){return fl(q.objectStore("LogsRequestsStore").index("newRequestV2"),{query:h,direction:"prev"},function(u){u.getValue()&&(k=u.getValue(),"NEW"===a&&(k.status="QUEUED",
u.update(k)))})}),3);
c.Bc=P();nm(c);return m.return(k)})}
function pm(a,b){var c;return w(function(d){if(1==d.h)return t(d,lm(b),2);c=d.i;return d.return(Uk(c,["LogsRequestsStore"],{mode:"readwrite",O:!0},function(e){var f=e.objectStore("LogsRequestsStore");return f.get(a).then(function(g){if(g)return g.status="QUEUED",Rk(f.h.put(g,void 0)).then(function(){return g})})}))})}
function qm(a,b,c){c=void 0===c?!0:c;var d;return w(function(e){if(1==e.h)return t(e,lm(b),2);d=e.i;return e.return(Uk(d,["LogsRequestsStore"],{mode:"readwrite",O:!0},function(f){var g=f.objectStore("LogsRequestsStore");return g.get(a).then(function(h){return h?(h.status="NEW",c&&(h.sendCount+=1),Rk(g.h.put(h,void 0)).then(function(){return h})):Kk.resolve(void 0)})}))})}
function rm(a,b){var c;return w(function(d){if(1==d.h)return t(d,lm(b),2);c=d.i;return d.return(c.delete("LogsRequestsStore",a))})}
function sm(a){var b,c;return w(function(d){if(1==d.h)return t(d,lm(a),2);b=d.i;c=P()-2592E6;return t(d,Uk(b,["LogsRequestsStore"],{mode:"readwrite",O:!0},function(e){return cl(e.objectStore("LogsRequestsStore"),{},function(f){if(f.getValue().timestamp<=c)return f.delete().then(function(){return f.continue()})})}),0)})}
function tm(){return w(function(a){return t(a,Kl(),0)})}
function nm(a){M("nwl_csi_killswitch")||.01>=Math.random()&&$l("nwl_transaction_latency_payload",a)}
;var um={},vm=im("ServiceWorkerLogsDatabase",{xa:(um.SWHealthLog={Va:1},um),jb:!0,upgrade:function(a,b){b(1)&&Vk(a,"SWHealthLog",{keyPath:"id",autoIncrement:!0}).h.createIndex("swHealthNewRequest",["interface","timestamp"],{unique:!1})},
version:1});function wm(a){return nl(vm(),a)}
function xm(a){var b,c;return w(function(d){if(1==d.h)return t(d,wm(a),2);b=d.i;c=P()-2592E6;return t(d,Uk(b,["SWHealthLog"],{mode:"readwrite",O:!0},function(e){return cl(e.objectStore("SWHealthLog"),{},function(f){if(f.getValue().timestamp<=c)return f.delete().then(function(){return f.continue()})})}),0)})}
function ym(a){var b;return w(function(c){if(1==c.h)return t(c,wm(a),2);b=c.i;return t(c,b.clear("SWHealthLog"),0)})}
;var zm={},Am=0;
function Bm(a){var b=void 0===b?"":b;if(a)if(b)gj(a,void 0,"POST",b,void 0);else if(A("USE_NET_AJAX_FOR_PING_TRANSPORT",!1))gj(a,void 0,"GET","",void 0);else{b:{try{var c=new cb({url:a});if(c.j&&c.i||c.l){var d=bc(cc(5,a)),e;if(!(e=!d||!d.endsWith("/aclk"))){var f=a.search(jc),g=ic(a,0,"ri",f);if(0>g)var h=null;else{var k=a.indexOf("&",g);if(0>k||k>f)k=f;g+=3;h=decodeURIComponent(a.substr(g,k-g).replace(/\+/g," "))}e="1"!==h}var m=!e;break b}}catch(u){}m=!1}if(m){b:{try{if(window.navigator&&window.navigator.sendBeacon&&
window.navigator.sendBeacon(a,"")){var q=!0;break b}}catch(u){}q=!1}b=q?!0:!1}else b=!1;b||Cm(a)}}
function Cm(a){var b=new Image,c=""+Am++;zm[c]=b;b.onload=b.onerror=function(){delete zm[c]};
b.src=a}
;function Dm(){this.h=new Map;this.i=!1}
function Em(){if(!Dm.h){var a=z("yt.networkRequestMonitor.instance")||new Dm;y("yt.networkRequestMonitor.instance",a,void 0);Dm.h=a}return Dm.h}
Dm.prototype.requestComplete=function(a,b){b&&(this.i=!0);a=this.removeParams(a);this.h.get(a)||this.h.set(a,b)};
Dm.prototype.isEndpointCFR=function(a){a=this.removeParams(a);return(a=this.h.get(a))?!1:!1===a&&this.i?!0:null};
Dm.prototype.removeParams=function(a){return a.split("?")[0]};
Dm.prototype.removeParams=Dm.prototype.removeParams;Dm.prototype.isEndpointCFR=Dm.prototype.isEndpointCFR;Dm.prototype.requestComplete=Dm.prototype.requestComplete;Dm.getInstance=Em;var Fm;function Gm(){Fm||(Fm=new Pj("yt.offline"));return Fm}
function Hm(a){if(M("offline_error_handling")){var b=Gm().get("errors",!0)||{};b[a.message]={name:a.name,stack:a.stack};a.level&&(b[a.message].level=a.level);Gm().set("errors",b,2592E3,!0)}}
function Im(){if(M("offline_error_handling")){var a=Gm().get("errors",!0);if(a){for(var b in a)if(a[b]){var c=new Q(b,"sent via offline_errors");c.name=a[b].name;c.stack=a[b].stack;c.level=a[b].level;ii(c)}Gm().set("errors",{},2592E3,!0)}}}
;var Jm=ci("network_polling_interval",3E4);function Km(){Ve.call(this);var a=this;this.S=0;this.A=this.m=!1;this.j=this.Za();M("use_shared_nsm")?(this.i=af(),M("use_shared_nsm_and_keep_yt_online_updated")&&(this.i.V("networkstatus-online",function(){a.j=!0;a.A&&Im()}),this.i.V("networkstatus-offline",function(){a.j=!1}))):(Lm(this),Mm(this))}
r(Km,Ve);function Nm(){if(!Km.h){var a=z("yt.networkStatusManager.instance")||new Km;y("yt.networkStatusManager.instance",a,void 0);Km.h=a}return Km.h}
l=Km.prototype;l.H=function(){if(M("use_shared_nsm")&&this.i){var a;return null==(a=this.i)?void 0:a.H()}return this.j};
l.ca=function(a){if(M("use_shared_nsm")&&this.i){var b;null!=(b=this.i)&&(b.i=a)}else a!==this.j&&(this.j=a)};
l.cc=function(a){!M("use_shared_nsm")&&(this.m=!0,void 0===a?0:a)&&(this.S||Om(this))};
l.Za=function(){var a=window.navigator.onLine;return void 0===a?!0:a};
l.Tb=function(){this.A=!0};
l.V=function(a,b){return M("use_shared_nsm")&&this.i?this.i.V(a,b):Ve.prototype.V.call(this,a,b)};
function Mm(a){window.addEventListener("online",function(){return w(function(b){if(1==b.h)return t(b,a.ea(),2);a.A&&Im();b.h=0})})}
function Lm(a){window.addEventListener("offline",function(){return w(function(b){return t(b,a.ea(),0)})})}
function Om(a){a.S=Yj(function(){return w(function(b){if(1==b.h)return a.j?a.Za()||!a.m?b.s(3):t(b,a.ea(),3):t(b,a.ea(),3);Om(a);b.h=0})},Jm)}
l.ea=function(a){var b=this;if(M("use_shared_nsm")&&this.i){var c=Ze(this.i,a);c.then(function(d){M("use_cfr_monitor")&&Em().requestComplete("generate_204",d)});
return c}return this.u?this.u:this.u=new Promise(function(d){var e,f,g,h;return w(function(k){switch(k.h){case 1:return e=window.AbortController?new window.AbortController:void 0,g=null==(f=e)?void 0:f.signal,h=!1,wa(k,2,3),e&&(b.M=bf.N(function(){e.abort()},a||2E4)),t(k,fetch("/generate_204",{method:"HEAD",
signal:g}),5);case 5:h=!0;case 3:za(k);M("use_cfr_monitor")&&Em().requestComplete("generate_204",h);b.u=void 0;b.M&&bf.T(b.M);h!==b.j&&(b.j=h,b.j&&b.m?We(b,"ytnetworkstatus-online"):b.m&&We(b,"ytnetworkstatus-offline"));d(h);Ca(k);break;case 2:ya(k),h=!1,k.s(3)}})})};
Km.prototype.sendNetworkCheckRequest=Km.prototype.ea;Km.prototype.listen=Km.prototype.V;Km.prototype.enableErrorFlushing=Km.prototype.Tb;Km.prototype.getWindowStatus=Km.prototype.Za;Km.prototype.monitorNetworkStatusChange=Km.prototype.cc;Km.prototype.networkStatusHint=Km.prototype.ca;Km.prototype.isNetworkAvailable=Km.prototype.H;Km.getInstance=Nm;function Pm(a){a=void 0===a?{}:a;Ve.call(this);var b=this;this.j=this.M=0;this.m="ytnetworkstatus-offline";this.u="ytnetworkstatus-online";M("use_shared_nsm")&&(this.m="networkstatus-offline",this.u="networkstatus-online");this.i=Nm();var c=z("yt.networkStatusManager.instance.monitorNetworkStatusChange").bind(this.i);c&&c(a.sb);a.zb&&(c=z("yt.networkStatusManager.instance.enableErrorFlushing").bind(this.i))&&c();if(c=z("yt.networkStatusManager.instance.listen").bind(this.i))a.Sa?(this.Sa=a.Sa,c(this.u,
function(){Qm(b,"publicytnetworkstatus-online")}),c(this.m,function(){Qm(b,"publicytnetworkstatus-offline")})):(c(this.u,function(){We(b,"publicytnetworkstatus-online")}),c(this.m,function(){We(b,"publicytnetworkstatus-offline")}))}
r(Pm,Ve);Pm.prototype.H=function(){var a=z("yt.networkStatusManager.instance.isNetworkAvailable");return a?a.bind(this.i)():!0};
Pm.prototype.ca=function(a){var b=z("yt.networkStatusManager.instance.networkStatusHint").bind(this.i);b&&b(a)};
Pm.prototype.ea=function(a){var b=this,c;return w(function(d){c=z("yt.networkStatusManager.instance.sendNetworkCheckRequest").bind(b.i);return M("skip_network_check_if_cfr")&&Em().isEndpointCFR("generate_204")?d.return(new Promise(function(e){var f;b.ca((null==(f=window.navigator)?void 0:f.onLine)||!0);e(b.H())})):c?d.return(c(a)):d.return(!0)})};
function Qm(a,b){a.Sa?a.j?(bf.T(a.M),a.M=bf.N(function(){a.A!==b&&(We(a,b),a.A=b,a.j=P())},a.Sa-(P()-a.j))):(We(a,b),a.A=b,a.j=P()):We(a,b)}
;var Rm;function Sm(){Ml.call(this,{K:{Qb:sm,oa:rm,vb:om,ac:pm,ib:qm,set:mm},J:Tm(),handleError:ii,qa:ji,da:Um,now:P,Lb:Hm,U:dk(),hb:"publicytnetworkstatus-online",gb:"publicytnetworkstatus-offline",Ka:!0,Ja:.1,Qa:ci("potential_esf_error_limit",10),D:M,wa:!(rk()&&Vm())});this.j=new Ad;M("networkless_immediately_drop_all_requests")&&tm();Ll("LogsDatabaseV2")}
r(Sm,Ml);function Wm(){var a=z("yt.networklessRequestController.instance");a||(a=new Sm,y("yt.networklessRequestController.instance",a,void 0),M("networkless_logging")&&Al().then(function(b){a.v=b;Nl(a);a.j.resolve();a.Ka&&Math.random()<=a.Ja&&a.v&&xm(a.v);M("networkless_immediately_drop_sw_health_store")&&Xm(a)}));
return a}
Sm.prototype.writeThenSend=function(a,b){b||(b={});rk()||(this.h=!1);Ml.prototype.writeThenSend.call(this,a,b)};
Sm.prototype.sendThenWrite=function(a,b,c){b||(b={});rk()||(this.h=!1);Ml.prototype.sendThenWrite.call(this,a,b,c)};
Sm.prototype.sendAndWrite=function(a,b){b||(b={});rk()||(this.h=!1);Ml.prototype.sendAndWrite.call(this,a,b)};
Sm.prototype.awaitInitialization=function(){return this.j.promise};
function Xm(a){var b;w(function(c){if(!a.v)throw b=Gk("clearSWHealthLogsDb"),b;return c.return(ym(a.v).catch(function(d){a.handleError(d)}))})}
function Um(a,b,c){M("use_cfr_monitor")&&Ym(a,b);var d;if(null==(d=b.postParams)?0:d.requestTimeMs)b.postParams.requestTimeMs=Math.round(P());c&&0===Object.keys(b).length?Bm(a):dj(a,b)}
function Tm(){Rm||(Rm=new Pm({zb:!0,sb:!0}));return Rm}
function Ym(a,b){var c=b.onError?b.onError:function(){};
b.onError=function(e,f){Em().requestComplete(a,!1);c(e,f)};
var d=b.onSuccess?b.onSuccess:function(){};
b.onSuccess=function(e,f){Em().requestComplete(a,!0);d(e,f)}}
function Vm(){return M("embeds_web_nwl_disable_nocookie")?"www.youtube-nocookie.com"!==dc(document.location.toString()):!0}
;var Zm=!1,$m=0,an=0,bn,cn=x.ytNetworklessLoggingInitializationOptions||{isNwlInitialized:Zm,potentialEsfErrorCounter:an};y("ytNetworklessLoggingInitializationOptions",cn,void 0);
function dn(){var a;w(function(b){switch(b.h){case 1:return t(b,Al(),2);case 2:a=b.i;if(!a||!rk()&&!M("nwl_init_require_datasync_id_killswitch")||!Vm()){b.s(0);break}Zm=!0;cn.isNwlInitialized=Zm;if(!M("use_new_nwl_initialization")){b.s(4);break}return t(b,Wm().awaitInitialization(),5);case 5:return b.return();case 4:return t(b,Ll("LogsDatabaseV2"),6);case 6:if(!(.1>=Math.random())){b.s(7);break}return t(b,sm(a),8);case 8:return t(b,xm(a),7);case 7:en();fn().H()&&gn();fn().V("publicytnetworkstatus-online",
gn);fn().V("publicytnetworkstatus-offline",hn);if(!M("networkless_immediately_drop_sw_health_store")){b.s(10);break}return t(b,jn(),10);case 10:if(M("networkless_immediately_drop_all_requests"))return t(b,tm(),0);b.s(0)}})}
function kn(a,b){function c(d){var e=fn().H();if(!ln()||!d||e&&M("vss_networkless_bypass_write"))mn(a,b);else{var f={url:a,options:b,timestamp:P(),status:"NEW",sendCount:0};mm(f,d).then(function(g){f.id=g;fn().H()&&nn(f)}).catch(function(g){nn(f);
fn().H()?ii(g):Hm(g)})}}
b=void 0===b?{}:b;M("skip_is_supported_killswitch")?Al().then(function(d){c(d)}):c(zl())}
function on(a,b){function c(d){if(ln()&&d){var e={url:a,options:b,timestamp:P(),status:"NEW",sendCount:0},f=!1,g=b.onSuccess?b.onSuccess:function(){};
e.options.onSuccess=function(k,m){M("use_cfr_monitor")&&Em().requestComplete(e.url,!0);void 0!==e.id?rm(e.id,d):f=!0;M("vss_network_hint")&&fn().ca(!0);g(k,m)};
if(M("use_cfr_monitor")){var h=b.onError?b.onError:function(){};
e.options.onError=function(k,m){Em().requestComplete(e.url,!1);h(k,m)}}mn(e.url,e.options);
mm(e,d).then(function(k){e.id=k;f&&rm(e.id,d)}).catch(function(k){fn().H()?ii(k):Hm(k)})}else mn(a,b)}
b=void 0===b?{}:b;M("skip_is_supported_killswitch")?Al().then(function(d){c(d)}):c(zl())}
function gn(){var a=zl();if(!a)throw Gk("throttleSend");$m||($m=bf.N(function(){var b;return w(function(c){if(1==c.h)return t(c,om("NEW",a),2);if(3!=c.h)return b=c.i,b?t(c,nn(b),3):(hn(),c.return());$m&&($m=0,gn());c.h=0})},100))}
function hn(){bf.T($m);$m=0}
function nn(a){var b,c,d;return w(function(e){switch(e.h){case 1:b=zl();if(!b)throw c=Gk("immediateSend"),c;if(void 0===a.id){e.s(2);break}return t(e,pm(a.id,b),3);case 3:(d=e.i)?a=d:ji(Error("The request cannot be found in the database."));case 2:if(pn(a,2592E6)){e.s(4);break}ji(Error("Networkless Logging: Stored logs request expired age limit"));if(void 0===a.id){e.s(5);break}return t(e,rm(a.id,b),5);case 5:return e.return();case 4:a.skipRetry||(a=qn(a));var f=a,g,h;if(null==f?0:null==(g=f.options)?
0:null==(h=g.postParams)?0:h.requestTimeMs)f.options.postParams.requestTimeMs=Math.round(P());a=f;if(!a){e.s(0);break}if(!a.skipRetry||void 0===a.id){e.s(8);break}return t(e,rm(a.id,b),8);case 8:mn(a.url,a.options,!!a.skipRetry),e.h=0}})}
function qn(a){var b=zl();if(!b)throw Gk("updateRequestHandlers");var c=a.options.onError?a.options.onError:function(){};
a.options.onError=function(e,f){var g,h,k;return w(function(m){switch(m.h){case 1:M("use_cfr_monitor")&&Em().requestComplete(a.url,!1);g=Tl(f);if(!(M("nwl_consider_error_code")&&g||!M("nwl_consider_error_code")&&rn()<=ci("potential_esf_error_limit",10))){m.s(2);break}if(M("skip_checking_network_on_cfr_failure")&&(!M("skip_checking_network_on_cfr_failure")||Em().isEndpointCFR(a.url))){m.s(3);break}return t(m,fn().ea(),3);case 3:if(fn().H()){m.s(2);break}c(e,f);if(!M("nwl_consider_error_code")||void 0===
(null==(h=a)?void 0:h.id)){m.s(6);break}return t(m,qm(a.id,b,!1),6);case 6:return m.return();case 2:if(M("nwl_consider_error_code")&&!g&&rn()>ci("potential_esf_error_limit",10))return m.return();z("ytNetworklessLoggingInitializationOptions")&&cn.potentialEsfErrorCounter++;an++;if(void 0===(null==(k=a)?void 0:k.id)){m.s(8);break}return 1>a.sendCount?t(m,qm(a.id,b),12):t(m,rm(a.id,b),8);case 12:bf.N(function(){fn().H()&&gn()},5E3);
case 8:c(e,f),m.h=0}})};
var d=a.options.onSuccess?a.options.onSuccess:function(){};
a.options.onSuccess=function(e,f){var g;return w(function(h){if(1==h.h)return M("use_cfr_monitor")&&Em().requestComplete(a.url,!0),void 0===(null==(g=a)?void 0:g.id)?h.s(2):t(h,rm(a.id,b),2);M("vss_network_hint")&&fn().ca(!0);d(e,f);h.h=0})};
return a}
function pn(a,b){a=a.timestamp;return P()-a>=b?!1:!0}
function en(){var a=zl();if(!a)throw Gk("retryQueuedRequests");om("QUEUED",a).then(function(b){b&&!pn(b,12E4)?bf.N(function(){return w(function(c){if(1==c.h)return void 0===b.id?c.s(2):t(c,qm(b.id,a),2);en();c.h=0})}):fn().H()&&gn()})}
function jn(){var a,b;return w(function(c){a=zl();if(!a)throw b=Gk("clearSWHealthLogsDb"),b;return c.return(ym(a).catch(function(d){ii(d)}))})}
function fn(){if(M("use_new_nwl"))return Tm();bn||(bn=new Pm({zb:!0,sb:!0}));return bn}
function mn(a,b,c){c&&0===Object.keys(b).length?Bm(a):dj(a,b)}
function ln(){return z("ytNetworklessLoggingInitializationOptions")?cn.isNwlInitialized:Zm}
function rn(){return z("ytNetworklessLoggingInitializationOptions")?cn.potentialEsfErrorCounter:an}
;function sn(a){var b=this;this.config_=null;a?this.config_=a:Ej()&&(this.config_=Fj());Yj(function(){Vj(b)},5E3)}
sn.prototype.isReady=function(){!this.config_&&Ej()&&(this.config_=Fj());return!!this.config_};
function Wj(a,b,c,d){function e(E){E=void 0===E?!1:E;var F;if(d.retry&&"www.youtube-nocookie.com"!=h&&(E||M("skip_ls_gel_retry")||"application/json"!==g.headers["Content-Type"]||(F=Tj(b,c,m,k)),F)){var H=g.onSuccess,R=g.onFetchSuccess;g.onSuccess=function(O,S){Uj(F);H(O,S)};
c.onFetchSuccess=function(O,S){Uj(F);R(O,S)}}try{E&&d.retry&&!d.Bb.bypassNetworkless?(g.method="POST",d.Bb.writeThenSend?M("use_new_nwl_wts")?Wm().writeThenSend(v,g):kn(v,g):M("use_new_nwl_saw")?Wm().sendAndWrite(v,g):on(v,g)):(g.method="POST",g.postParams||(g.postParams={}),dj(v,g))}catch(O){if("InvalidAccessError"==O.name)F&&(Uj(F),F=0),ji(Error("An extension is blocking network request."));
else throw O;}F&&Yj(function(){Vj(a)},5E3)}
!A("VISITOR_DATA")&&"visitor_id"!==b&&.01>Math.random()&&ji(new Q("Missing VISITOR_DATA when sending innertube request.",b,c,d));if(!a.isReady()){var f=new Q("innertube xhrclient not ready",b,c,d);ii(f);throw f;}var g={headers:d.headers||{},method:"POST",postParams:c,postBody:d.postBody,postBodyFormat:d.postBodyFormat||"JSON",onTimeout:function(){d.onTimeout()},
onFetchTimeout:d.onTimeout,onSuccess:function(E,F){if(d.onSuccess)d.onSuccess(F)},
onFetchSuccess:function(E){if(d.onSuccess)d.onSuccess(E)},
onError:function(E,F){if(d.onError)d.onError(F)},
onFetchError:function(E){if(d.onError)d.onError(E)},
timeout:d.timeout,withCredentials:!0};g.headers["Content-Type"]||(g.headers["Content-Type"]="application/json");var h="";(f=a.config_.Xb)&&(h=f);var k=a.config_.Zb||!1,m=Nj(k,h,d);Object.assign(g.headers,m);(f=g.headers.Authorization)&&!h&&(g.headers["x-origin"]=window.location.origin);var q="/youtubei/"+a.config_.innertubeApiVersion+"/"+b,u={alt:"json"},C=a.config_.Yb&&f;C=C&&f.startsWith("Bearer");C||(u.key=a.config_.innertubeApiKey);var v=vi(""+h+q,u||{},!0);M("use_new_nwl")&&Wm().h||!M("use_new_nwl")&&
ln()?yl().then(function(E){e(E)}):e(!1)}
;var tn={appSettingsCaptured:!0,visualElementAttached:!0,visualElementGestured:!0,visualElementHidden:!0,visualElementShown:!0,flowEvent:!0,visualElementStateChanged:!0,playbackAssociated:!0,youThere:!0,accountStateChangeSignedIn:!0,accountStateChangeSignedOut:!0},un={latencyActionBaselined:!0,latencyActionInfo:!0,latencyActionTicked:!0,bedrockRepetitiveActionTimed:!0,adsClientStateChange:!0,streamzIncremented:!0,mdxDialAdditionalDataUpdateEvent:!0,tvhtml5WatchKeyEvent:!0,tvhtml5VideoSeek:!0,tokenRefreshEvent:!0,
adNotify:!0,adNotifyFilled:!0,tvhtml5LaunchUrlComponentChanged:!0,bedrockResourceConsumptionSnapshot:!0,deviceStartupMetrics:!0,mdxSignIn:!0,tvhtml5KeyboardLogging:!0,tvhtml5StartupSoundEvent:!0,tvhtml5LiveChatStatus:!0,tvhtml5DeviceStorageStatus:!0,tvhtml5LocalStorage:!0,directSignInEvent:!0,finalPayload:!0,tvhtml5SearchCompleted:!0,tvhtml5KeyboardPerformance:!0,adNotifyFailure:!0,latencyActionSpan:!0,tvhtml5AccountDialogOpened:!0,tvhtml5ApiTest:!0};var vn=0,wn=vc?"webkit":uc?"moz":sc?"ms":rc?"o":"";y("ytDomDomGetNextId",z("ytDomDomGetNextId")||function(){return++vn},void 0);var xn={stopImmediatePropagation:1,stopPropagation:1,preventMouseEvent:1,preventManipulation:1,preventDefault:1,layerX:1,layerY:1,screenX:1,screenY:1,scale:1,rotation:1,webkitMovementX:1,webkitMovementY:1};
function yn(a){this.type="";this.state=this.source=this.data=this.currentTarget=this.relatedTarget=this.target=null;this.charCode=this.keyCode=0;this.metaKey=this.shiftKey=this.ctrlKey=this.altKey=!1;this.rotation=this.clientY=this.clientX=0;this.scale=1;this.changedTouches=this.touches=null;try{if(a=a||window.event){this.event=a;for(var b in a)b in xn||(this[b]=a[b]);this.scale=a.scale;this.rotation=a.rotation;var c=a.target||a.srcElement;c&&3==c.nodeType&&(c=c.parentNode);this.target=c;var d=a.relatedTarget;
if(d)try{d=d.nodeName?d:null}catch(e){d=null}else"mouseover"==this.type?d=a.fromElement:"mouseout"==this.type&&(d=a.toElement);this.relatedTarget=d;this.clientX=void 0!=a.clientX?a.clientX:a.pageX;this.clientY=void 0!=a.clientY?a.clientY:a.pageY;this.keyCode=a.keyCode?a.keyCode:a.which;this.charCode=a.charCode||("keypress"==this.type?this.keyCode:0);this.altKey=a.altKey;this.ctrlKey=a.ctrlKey;this.shiftKey=a.shiftKey;this.metaKey=a.metaKey;this.h=a.pageX;this.i=a.pageY}}catch(e){}}
function zn(a){if(document.body&&document.documentElement){var b=document.body.scrollTop+document.documentElement.scrollTop;a.h=a.clientX+(document.body.scrollLeft+document.documentElement.scrollLeft);a.i=a.clientY+b}}
yn.prototype.preventDefault=function(){this.event&&(this.event.returnValue=!1,this.event.preventDefault&&this.event.preventDefault())};
yn.prototype.stopPropagation=function(){this.event&&(this.event.cancelBubble=!0,this.event.stopPropagation&&this.event.stopPropagation())};
yn.prototype.stopImmediatePropagation=function(){this.event&&(this.event.cancelBubble=!0,this.event.stopImmediatePropagation&&this.event.stopImmediatePropagation())};var ob=x.ytEventsEventsListeners||{};y("ytEventsEventsListeners",ob,void 0);var An=x.ytEventsEventsCounter||{count:0};y("ytEventsEventsCounter",An,void 0);
function Bn(a,b,c,d){d=void 0===d?{}:d;a.addEventListener&&("mouseenter"!=b||"onmouseenter"in document?"mouseleave"!=b||"onmouseenter"in document?"mousewheel"==b&&"MozBoxSizing"in document.documentElement.style&&(b="MozMousePixelScroll"):b="mouseout":b="mouseover");return nb(function(e){var f="boolean"===typeof e[4]&&e[4]==!!d,g=Ra(e[4])&&Ra(d)&&ub(e[4],d);return!!e.length&&e[0]==a&&e[1]==b&&e[2]==c&&(f||g)})}
var Cn=db(function(){var a=!1;try{var b=Object.defineProperty({},"capture",{get:function(){a=!0}});
window.addEventListener("test",null,b)}catch(c){}return a});
function Dn(a,b,c,d){d=void 0===d?{}:d;if(!a||!a.addEventListener&&!a.attachEvent)return"";var e=Bn(a,b,c,d);if(e)return e;e=++An.count+"";var f=!("mouseenter"!=b&&"mouseleave"!=b||!a.addEventListener||"onmouseenter"in document);var g=f?function(h){h=new yn(h);if(!Nd(h.relatedTarget,function(k){return k==a}))return h.currentTarget=a,h.type=b,c.call(a,h)}:function(h){h=new yn(h);
h.currentTarget=a;return c.call(a,h)};
g=hi(g);a.addEventListener?("mouseenter"==b&&f?b="mouseover":"mouseleave"==b&&f?b="mouseout":"mousewheel"==b&&"MozBoxSizing"in document.documentElement.style&&(b="MozMousePixelScroll"),Cn()||"boolean"===typeof d?a.addEventListener(b,g,d):a.addEventListener(b,g,!!d.capture)):a.attachEvent("on"+b,g);ob[e]=[a,b,c,g,d];return e}
function En(a){a&&("string"==typeof a&&(a=[a]),fb(a,function(b){if(b in ob){var c=ob[b],d=c[0],e=c[1],f=c[3];c=c[4];d.removeEventListener?Cn()||"boolean"===typeof c?d.removeEventListener(e,f,c):d.removeEventListener(e,f,!!c.capture):d.detachEvent&&d.detachEvent("on"+e,f);delete ob[b]}}))}
;var Fn=window.ytcsi&&window.ytcsi.now?window.ytcsi.now:window.performance&&window.performance.timing&&window.performance.now&&window.performance.timing.navigationStart?function(){return window.performance.timing.navigationStart+window.performance.now()}:function(){return(new Date).getTime()};function Gn(a){this.L=a;this.i=null;this.m=0;this.A=null;this.u=0;this.j=[];for(a=0;4>a;a++)this.j.push(0);this.l=0;this.S=Dn(window,"mousemove",Xa(this.Y,this));a=Xa(this.M,this);"function"===typeof a&&(a=hi(a));this.fa=window.setInterval(a,25)}
$a(Gn,K);Gn.prototype.Y=function(a){void 0===a.h&&zn(a);var b=a.h;void 0===a.i&&zn(a);this.i=new Jd(b,a.i)};
Gn.prototype.M=function(){if(this.i){var a=Fn();if(0!=this.m){var b=this.A,c=this.i,d=b.x-c.x;b=b.y-c.y;d=Math.sqrt(d*d+b*b)/(a-this.m);this.j[this.l]=.5<Math.abs((d-this.u)/this.u)?1:0;for(c=b=0;4>c;c++)b+=this.j[c]||0;3<=b&&this.L();this.u=d}this.m=a;this.A=this.i;this.l=(this.l+1)%4}};
Gn.prototype.I=function(){window.clearInterval(this.fa);En(this.S)};var Hn={};
function In(a){var b=void 0===a?{}:a;a=void 0===b.fc?!1:b.fc;b=void 0===b.Ub?!0:b.Ub;if(null==z("_lact",window)){var c=parseInt(A("LACT"),10);c=isFinite(c)?Date.now()-Math.max(c,0):-1;y("_lact",c,window);y("_fact",c,window);-1==c&&Jn();Dn(document,"keydown",Jn);Dn(document,"keyup",Jn);Dn(document,"mousedown",Jn);Dn(document,"mouseup",Jn);a?Dn(window,"touchmove",function(){Kn("touchmove",200)},{passive:!0}):(Dn(window,"resize",function(){Kn("resize",200)}),b&&Dn(window,"scroll",function(){Kn("scroll",200)}));
new Gn(function(){Kn("mouse",100)});
Dn(document,"touchstart",Jn,{passive:!0});Dn(document,"touchend",Jn,{passive:!0})}}
function Kn(a,b){Hn[a]||(Hn[a]=!0,bf.N(function(){Jn();Hn[a]=!1},b))}
function Jn(){null==z("_lact",window)&&In();var a=Date.now();y("_lact",a,window);-1==z("_fact",window)&&y("_fact",a,window);(a=z("ytglobal.ytUtilActivityCallback_"))&&a()}
function Ln(){var a=z("_lact",window);return null==a?-1:Math.max(Date.now()-a,0)}
;var Mn=x.ytPubsubPubsubInstance||new L,Nn=x.ytPubsubPubsubSubscribedKeys||{},On=x.ytPubsubPubsubTopicToKeys||{},Pn=x.ytPubsubPubsubIsSynchronous||{};function Qn(a,b){var c=Rn();if(c&&b){var d=c.subscribe(a,function(){var e=arguments;var f=function(){Nn[d]&&b.apply&&"function"==typeof b.apply&&b.apply(window,e)};
try{Pn[a]?f():Ci(f,0)}catch(g){ii(g)}},void 0);
Nn[d]=!0;On[a]||(On[a]=[]);On[a].push(d);return d}return 0}
function Sn(a){var b=Rn();b&&("number"===typeof a?a=[a]:"string"===typeof a&&(a=[parseInt(a,10)]),fb(a,function(c){b.unsubscribeByKey(c);delete Nn[c]}))}
function Tn(a,b){var c=Rn();c&&c.publish.apply(c,arguments)}
function Un(a){var b=Rn();if(b)if(b.clear(a),a)lo(a);else for(var c in On)lo(c)}
function Rn(){return x.ytPubsubPubsubInstance}
function lo(a){On[a]&&(a=On[a],fb(a,function(b){Nn[b]&&delete Nn[b]}),a.length=0)}
L.prototype.subscribe=L.prototype.subscribe;L.prototype.unsubscribeByKey=L.prototype.ta;L.prototype.publish=L.prototype.ja;L.prototype.clear=L.prototype.clear;y("ytPubsubPubsubInstance",Mn,void 0);y("ytPubsubPubsubTopicToKeys",On,void 0);y("ytPubsubPubsubIsSynchronous",Pn,void 0);y("ytPubsubPubsubSubscribedKeys",Nn,void 0);var Ko=ci("initial_gel_batch_timeout",2E3),Lo=Math.pow(2,16)-1,Mo=void 0;function No(){this.j=this.h=this.i=0}
var Oo=new No,Po=new No,Qo=!0,Ro=x.ytLoggingTransportGELQueue_||new Map;y("ytLoggingTransportGELQueue_",Ro,void 0);var So=x.ytLoggingTransportGELProtoQueue_||new Map;y("ytLoggingTransportGELProtoQueue_",So,void 0);var To=x.ytLoggingTransportTokensToCttTargetIds_||{};y("ytLoggingTransportTokensToCttTargetIds_",To,void 0);var Uo=x.ytLoggingTransportTokensToJspbCttTargetIds_||{};y("ytLoggingTransportTokensToJspbCttTargetIds_",Uo,void 0);
function Vo(a,b){if("log_event"===a.endpoint){var c=Wo(a),d=Ro.get(c)||[];Ro.set(c,d);d.push(a.payload);Xo(b,d,c)}}
function Yo(a,b){if("log_event"===a.endpoint){var c=Wo(a,!0),d=So.get(c)||[];So.set(c,d);a=td(a.payload);d.push(a);Xo(b,d,c,!0)}}
function Xo(a,b,c,d){d=void 0===d?!1:d;a&&(Mo=new a);a=ci("tvhtml5_logging_max_batch")||ci("web_logging_max_batch")||100;var e=P(),f=d?Po.j:Oo.j;b.length>=a?Zo({writeThenSend:!0},M("flush_only_full_queue")?c:void 0,d):10<=e-f&&($o(d),d?Po.j=e:Oo.j=e)}
function ap(a,b){if("log_event"===a.endpoint){var c=Wo(a),d=new Map;d.set(c,[a.payload]);b&&(Mo=new b);return new $f(function(e){Mo&&Mo.isReady()?bp(d,e,{bypassNetworkless:!0},!0):e()})}}
function cp(a,b){if("log_event"===a.endpoint){var c=Wo(a,!0),d=new Map;d.set(c,[td(a.payload)]);b&&(Mo=new b);return new $f(function(e){Mo&&Mo.isReady()?dp(d,e,{bypassNetworkless:!0},!0):e()})}}
function Wo(a,b){var c="";if(a.va)c="visitorOnlyApprovedKey";else if(a.cttAuthInfo){if(void 0===b?0:b){b=a.cttAuthInfo.token;c=a.cttAuthInfo;var d=new Gh;c.videoId?d.setVideoId(c.videoId):c.playlistId&&jd(d,2,Hh,c.playlistId);Uo[b]=d}else b=a.cttAuthInfo,c={},b.videoId?c.videoId=b.videoId:b.playlistId&&(c.playlistId=b.playlistId),To[a.cttAuthInfo.token]=c;c=a.cttAuthInfo.token}return c}
function Zo(a,b,c){a=void 0===a?{}:a;c=void 0===c?!1:c;new $f(function(d){c?(Di(Po.i),Di(Po.h),Po.h=0):(Di(Oo.i),Di(Oo.h),Oo.h=0);if(Mo&&Mo.isReady())if(void 0!==b)if(c){var e=new Map,f=So.get(b)||[];e.set(b,f);dp(e,d,a);So.delete(b)}else e=new Map,f=Ro.get(b)||[],e.set(b,f),bp(e,d,a),Ro.delete(b);else c?(dp(So,d,a),So.clear()):(bp(Ro,d,a),Ro.clear());else $o(c),d()})}
function $o(a){a=void 0===a?!1:a;if(M("web_gel_timeout_cap")&&(!a&&!Oo.h||a&&!Po.h)){var b=Ci(function(){Zo({writeThenSend:!0},void 0,a)},6E4);
a?Po.h=b:Oo.h=b}Di(a?Po.i:Oo.i);b=A("LOGGING_BATCH_TIMEOUT",ci("web_gel_debounce_ms",1E4));M("shorten_initial_gel_batch_timeout")&&Qo&&(b=Ko);b=Ci(function(){Zo({writeThenSend:!0},void 0,a)},b);
a?Po.i=b:Oo.i=b}
function bp(a,b,c,d){var e=Mo;c=void 0===c?{}:c;var f=Math.round(P()),g=a.size;a=p(a);for(var h=a.next();!h.done;h=a.next()){var k=p(h.value);h=k.next().value;var m=k=k.next().value;k=wb({context:Gj(e.config_||Fj())});k.events=m;(m=To[h])&&ep(k,h,m);delete To[h];h="visitorOnlyApprovedKey"===h;fp(k,f,h);gp(c);Wj(e,"log_event",k,hp(c,h,function(){g--;g||b()},function(){g--;
g||b()},d));
Qo=!1}}
function dp(a,b,c,d){var e=Mo;c=void 0===c?{}:c;var f=Math.round(P()),g=a.size;a=p(a);for(var h=a.next();!h.done;h=a.next()){var k=p(h.value);h=k.next().value;var m=k=k.next().value;k=new Ih;var q=Mj(e.config_||Fj());I(k,1,q);m=ip(m);for(q=0;q<m.length;q++)od(k,3,Dh,m[q]);(m=Uo[h])&&jp(k,h,m);delete Uo[h];h="visitorOnlyApprovedKey"===h;kp(k,f,h);gp(c);k=td(k);h=hp(c,h,function(){g--;g||b()},function(){g--;
g||b()},d);
h.headers={"Content-Type":"application/json+protobuf"};h.postBodyFormat="JSPB";h.postBody=k;Wj(e,"log_event","",h);Qo=!1}}
function gp(a){M("always_send_and_write")&&(a.writeThenSend=!1)}
function hp(a,b,c,d,e){return{retry:!0,onSuccess:c,onError:d,Bb:a,va:b,Eo:!!e,headers:{},postBodyFormat:"",postBody:""}}
function fp(a,b,c){a.requestTimeMs=String(b);M("unsplit_gel_payloads_in_logs")&&(a.unsplitGelPayloadsInLogs=!0);!c&&(b=A("EVENT_ID",void 0))&&(c=lp(),a.serializedClientEventId={serializedEventId:b,clientCounter:String(c)})}
function kp(a,b,c){D(a,2,b);if(!c&&(b=A("EVENT_ID",void 0))){c=lp();var d=new Fh;D(d,1,b);D(d,2,c);I(a,5,d)}}
function lp(){var a=A("BATCH_CLIENT_COUNTER",void 0)||0;a||(a=Math.floor(Math.random()*Lo/2));a++;a>Lo&&(a=1);Zh("BATCH_CLIENT_COUNTER",a);return a}
function ep(a,b,c){if(c.videoId)var d="VIDEO";else if(c.playlistId)d="PLAYLIST";else return;a.credentialTransferTokenTargetId=c;a.context=a.context||{};a.context.user=a.context.user||{};a.context.user.credentialTransferTokens=[{token:b,scope:d}]}
function jp(a,b,c){if(hd(c,1===kd(c,Hh)?1:-1))var d=1;else if(c.getPlaylistId())d=2;else return;I(a,4,c);a=ld(a,Ug,1)||new Ug;c=ld(a,Sg,3)||new Sg;var e=new Rg;e.setToken(b);D(e,1,d);od(c,12,Rg,e);I(a,3,c)}
function ip(a){for(var b=[],c=0;c<a.length;c++)try{var d=b,e=d.push;var f=String(a[c]),g=Dh;if(null==f||""==f)var h=new g;else{var k=JSON.parse(f);Array.isArray(k)||qd(Error("Expected to deserialize an Array but got "+Pa(k)+": "+k));sd=k;var m=new g(k);sd=null;h=m}e.call(d,h)}catch(q){ii(new Q("Transport failed to deserialize "+String(a[c])))}return b}
;var mp=x.ytLoggingGelSequenceIdObj_||{};y("ytLoggingGelSequenceIdObj_",mp,void 0);
function np(a,b,c,d){d=void 0===d?{}:d;if(M("lr_drop_other_and_business_payloads")){if(un[a]||tn[a])return}else if(M("lr_drop_other_payloads")&&un[a])return;var e={},f=Math.round(d.timestamp||P());e.eventTimeMs=f<Number.MAX_SAFE_INTEGER?f:0;e[a]=b;a=Ln();e.context={lastActivityMs:String(d.timestamp||!isFinite(a)?-1:a)};M("log_sequence_info_on_gel_web")&&d.W&&(a=e.context,b=d.W,b={index:op(b),groupKey:b},a.sequence=b,d.tb&&delete mp[d.W]);(d.oc?ap:Vo)({endpoint:"log_event",payload:e,cttAuthInfo:d.cttAuthInfo,
va:d.va},c)}
function op(a){mp[a]=a in mp?mp[a]+1:0;return mp[a]}
;function ok(a,b,c){c=void 0===c?{}:c;var d=sn;A("ytLoggingEventsDefaultDisabled",!1)&&sn==sn&&(d=null);np(a,b,d,c)}
;var pp=[{fb:function(a){return"Cannot read property '"+a.key+"'"},
Pa:{Error:[{regexp:/(Permission denied) to access property "([^']+)"/,groups:["reason","key"]}],TypeError:[{regexp:/Cannot read property '([^']+)' of (null|undefined)/,groups:["key","value"]},{regexp:/\u65e0\u6cd5\u83b7\u53d6\u672a\u5b9a\u4e49\u6216 (null|undefined) \u5f15\u7528\u7684\u5c5e\u6027\u201c([^\u201d]+)\u201d/,groups:["value","key"]},{regexp:/\uc815\uc758\ub418\uc9c0 \uc54a\uc74c \ub610\ub294 (null|undefined) \ucc38\uc870\uc778 '([^']+)' \uc18d\uc131\uc744 \uac00\uc838\uc62c \uc218 \uc5c6\uc2b5\ub2c8\ub2e4./,
groups:["value","key"]},{regexp:/No se puede obtener la propiedad '([^']+)' de referencia nula o sin definir/,groups:["key"]},{regexp:/Unable to get property '([^']+)' of (undefined or null) reference/,groups:["key","value"]},{regexp:/(null) is not an object \(evaluating '(?:([^.]+)\.)?([^']+)'\)/,groups:["value","base","key"]}]}},{fb:function(a){return"Cannot call '"+a.key+"'"},
Pa:{TypeError:[{regexp:/(?:([^ ]+)?\.)?([^ ]+) is not a function/,groups:["base","key"]},{regexp:/([^ ]+) called on (null or undefined)/,groups:["key","value"]},{regexp:/Object (.*) has no method '([^ ]+)'/,groups:["base","key"]},{regexp:/Object doesn't support property or method '([^ ]+)'/,groups:["key"]},{regexp:/\u30aa\u30d6\u30b8\u30a7\u30af\u30c8\u306f '([^']+)' \u30d7\u30ed\u30d1\u30c6\u30a3\u307e\u305f\u306f\u30e1\u30bd\u30c3\u30c9\u3092\u30b5\u30dd\u30fc\u30c8\u3057\u3066\u3044\u307e\u305b\u3093/,
groups:["key"]},{regexp:/\uac1c\uccb4\uac00 '([^']+)' \uc18d\uc131\uc774\ub098 \uba54\uc11c\ub4dc\ub97c \uc9c0\uc6d0\ud558\uc9c0 \uc54a\uc2b5\ub2c8\ub2e4./,groups:["key"]}]}},{fb:function(a){return a.key+" is not defined"},
Pa:{ReferenceError:[{regexp:/(.*) is not defined/,groups:["key"]},{regexp:/Can't find variable: (.*)/,groups:["key"]}]}}];var rp={ha:[],ga:[{ka:qp,weight:500}]};function qp(a){if("JavaException"===a.name)return!0;a=a.stack;return a.includes("chrome://")||a.includes("chrome-extension://")||a.includes("moz-extension://")}
;function sp(){this.ga=[];this.ha=[]}
var tp;function up(){if(!tp){var a=tp=new sp;a.ha.length=0;a.ga.length=0;rp.ha&&a.ha.push.apply(a.ha,rp.ha);rp.ga&&a.ga.push.apply(a.ga,rp.ga)}return tp}
;var vp=new L;function wp(a){function b(){return a.charCodeAt(d++)}
var c=a.length,d=0;do{var e=xp(b);if(Infinity===e)break;var f=e>>3;switch(e&7){case 0:e=xp(b);if(2===f)return e;break;case 1:if(2===f)return;d+=8;break;case 2:e=xp(b);if(2===f)return a.substr(d,e);d+=e;break;case 5:if(2===f)return;d+=4;break;default:return}}while(d<c)}
function xp(a){var b=a(),c=b&127;if(128>b)return c;b=a();c|=(b&127)<<7;if(128>b)return c;b=a();c|=(b&127)<<14;if(128>b)return c;b=a();return 128>b?c|(b&127)<<21:Infinity}
;function yp(a,b,c,d){if(a)if(Array.isArray(a)){var e=d;for(d=0;d<a.length&&!(a[d]&&(e+=zp(d,a[d],b,c),500<e));d++);d=e}else if("object"===typeof a)for(e in a){if(a[e]){var f=a[e];var g=b;var h=c;g="string"!==typeof f||"clickTrackingParams"!==e&&"trackingParams"!==e?0:(f=wp(atob(f.replace(/-/g,"+").replace(/_/g,"/"))))?zp(e+".ve",f,g,h):0;d+=g;d+=zp(e,a[e],b,c);if(500<d)break}}else c[b]=Ap(a),d+=c[b].length;else c[b]=Ap(a),d+=c[b].length;return d}
function zp(a,b,c,d){c+="."+a;a=Ap(b);d[c]=a;return c.length+a.length}
function Ap(a){try{return("string"===typeof a?a:String(JSON.stringify(a))).substr(0,500)}catch(b){return"unable to serialize "+typeof a+" ("+b.message+")"}}
;var Bp=x.ytLoggingGelSequenceIdObj_||{};y("ytLoggingGelSequenceIdObj_",Bp,void 0);function Cp(a,b,c){c=void 0===c?{}:c;var d=Math.round(c.timestamp||P());D(a,1,d<Number.MAX_SAFE_INTEGER?d:0);var e=Ln();d=new Ch;D(d,1,c.timestamp||!isFinite(e)?-1:e);if(M("log_sequence_info_on_gel_web")&&c.W){e=c.W;var f=op(e),g=new Bh;D(g,2,f);D(g,1,e);I(d,3,g);c.tb&&delete Bp[c.W]}I(a,33,d);(c.oc?cp:Yo)({endpoint:"log_event",payload:a,cttAuthInfo:c.cttAuthInfo,va:c.va},b)}
;function Dp(a,b){b=void 0===b?{}:b;var c=!1;A("ytLoggingEventsDefaultDisabled",!1)&&sn===sn&&(c=!0);Cp(a,c?null:sn,b)}
;function Ep(a,b,c){var d=new Dh;nd(d,72,Eh,a);c?Cp(d,c,b):Dp(d,b)}
function Fp(a,b,c){var d=new Dh;nd(d,73,Eh,a);c?Cp(d,c,b):Dp(d,b)}
function Gp(a,b,c){var d=new Dh;nd(d,78,Eh,a);c?Cp(d,c,b):Dp(d,b)}
function Hp(a,b,c){var d=new Dh;nd(d,208,Eh,a);c?Cp(d,c,b):Dp(d,b)}
function Ip(a,b,c){var d=new Dh;nd(d,156,Eh,a);c?Cp(d,c,b):Dp(d,b)}
function Jp(a,b,c){var d=new Dh;nd(d,215,Eh,a);c?Cp(d,c,b):Dp(d,b)}
function Kp(a,b,c){var d=new Dh;nd(d,111,Eh,a);c?Cp(d,c,b):Dp(d,b)}
;var Lp=new Set,Mp=0,Np=0,Op=0,Pp=[],Qp=["PhantomJS","Googlebot","TO STOP THIS SECURITY SCAN go/scan"];function nk(a){Rp(a)}
function Sp(a){Rp(a,"WARNING")}
function Rp(a,b,c,d,e,f){f=void 0===f?{}:f;f.name=c||A("INNERTUBE_CONTEXT_CLIENT_NAME",1);f.version=d||A("INNERTUBE_CONTEXT_CLIENT_VERSION",void 0);var g=f||{},h=void 0===b?"ERROR":b;h=void 0===h?"ERROR":h;if(a){a.hasOwnProperty("level")&&a.level&&(h=a.level);if(M("console_log_js_exceptions")){var k=[];k.push("Name: "+a.name);k.push("Message: "+a.message);a.hasOwnProperty("params")&&k.push("Error Params: "+JSON.stringify(a.params));a.hasOwnProperty("args")&&k.push("Error args: "+JSON.stringify(a.args));
k.push("File name: "+a.fileName);k.push("Stacktrace: "+a.stack);window.console.log(k.join("\n"),a)}if(!(5<=Mp)){var m=Pp,q=te(a),u=q.message||"Unknown Error",C=q.name||"UnknownError",v=q.stack||a.i||"Not available";if(v.startsWith(C+": "+u)){var E=v.split("\n");E.shift();v=E.join("\n")}var F=q.lineNumber||"Not available",H=q.fileName||"Not available",R=v,O=0;if(a.hasOwnProperty("args")&&a.args&&a.args.length)for(var S=0;S<a.args.length&&!(O=yp(a.args[S],"params."+S,g,O),500<=O);S++);else if(a.hasOwnProperty("params")&&
a.params){var ia=a.params;if("object"===typeof a.params)for(var N in ia){if(ia[N]){var Aa="params."+N,Na=Ap(ia[N]);g[Aa]=Na;O+=Aa.length+Na.length;if(500<O)break}}else g.params=Ap(ia)}if(m.length)for(var ta=0;ta<m.length&&!(O=yp(m[ta],"params.context."+ta,g,O),500<=O);ta++);navigator.vendor&&!g.hasOwnProperty("vendor")&&(g["device.vendor"]=navigator.vendor);var G={message:u,name:C,lineNumber:F,fileName:H,stack:R,params:g,sampleWeight:1},Ba=Number(a.columnNumber);isNaN(Ba)||(G.lineNumber=G.lineNumber+
":"+Ba);if("IGNORED"===a.level)var sa=0;else a:{for(var Vn=up(),Wn=p(Vn.ha),Gi=Wn.next();!Gi.done;Gi=Wn.next()){var Xn=Gi.value;if(G.message&&G.message.match(Xn.Jo)){sa=Xn.weight;break a}}for(var Yn=p(Vn.ga),Hi=Yn.next();!Hi.done;Hi=Yn.next()){var Zn=Hi.value;if(Zn.ka(G)){sa=Zn.weight;break a}}sa=1}G.sampleWeight=sa;for(var $n=p(pp),Ii=$n.next();!Ii.done;Ii=$n.next()){var Ji=Ii.value;if(Ji.Pa[G.name])for(var ao=p(Ji.Pa[G.name]),Ki=ao.next();!Ki.done;Ki=ao.next()){var bo=Ki.value,Sf=G.message.match(bo.regexp);
if(Sf){G.params["params.error.original"]=Sf[0];for(var Li=bo.groups,co={},cd=0;cd<Li.length;cd++)co[Li[cd]]=Sf[cd+1],G.params["params.error."+Li[cd]]=Sf[cd+1];G.message=Ji.fb(co);break}}}G.params||(G.params={});var eo=up();G.params["params.errorServiceSignature"]="msg="+eo.ha.length+"&cb="+eo.ga.length;G.params["params.serviceWorker"]="false";x.document&&x.document.querySelectorAll&&(G.params["params.fscripts"]=String(document.querySelectorAll("script:not([nonce])").length));Db("sample").constructor!==
Bb&&(G.params["params.fconst"]="true");window.yterr&&"function"===typeof window.yterr&&window.yterr(G);if(0!==G.sampleWeight&&!Lp.has(G.message)){if("ERROR"===h){vp.ja("handleError",G);if(M("record_app_crashed_web")&&0===Op&&1===G.sampleWeight)if(Op++,M("errors_via_jspb")){var Mi=new gh;D(Mi,1,1);if(!M("report_client_error_with_app_crash_ks")){var fo=new bh;D(fo,1,G.message);var go=new ch;I(go,3,fo);var ho=new dh;I(ho,5,go);var io=new eh;I(io,9,ho);I(Mi,4,io)}var jo=new Dh;nd(jo,20,Eh,Mi);Dp(jo,void 0)}else{var ko=
{appCrashType:"APP_CRASH_TYPE_BREAKPAD"};M("report_client_error_with_app_crash_ks")||(ko.systemHealth={crashData:{clientError:{logMessage:{message:G.message}}}});ok("appCrashed",ko)}Np++}else"WARNING"===h&&vp.ja("handleWarning",G);if(M("kevlar_gel_error_routing"))a:{var ae=h;if(M("errors_via_jspb")){if(Tp())var mo=void 0;else{var dd=new Zg;D(dd,1,G.stack);G.fileName&&D(dd,4,G.fileName);var sb=G.lineNumber&&G.lineNumber.split?G.lineNumber.split(":"):[];0!==sb.length&&(1!==sb.length||isNaN(Number(sb[0]))?
2!==sb.length||isNaN(Number(sb[0]))||isNaN(Number(sb[1]))||(D(dd,2,Number(sb[0])),D(dd,3,Number(sb[1]))):D(dd,2,Number(sb[0])));var nc=new bh;D(nc,1,G.message);D(nc,3,G.name);D(nc,6,G.sampleWeight);"ERROR"===ae?D(nc,2,2):"WARNING"===ae?D(nc,2,1):D(nc,2,0);var Ni=new $g;D(Ni,1,!0);nd(Ni,3,ah,dd);var Nb=new Wg;D(Nb,3,window.location.href);for(var no=A("FEXP_EXPERIMENTS",[]),Oi=0;Oi<no.length;Oi++){var Ft=no[Oi];Wc(Nb);id(Nb,5).push(Ft)}var Pi=A("LATEST_ECATCHER_SERVICE_TRACKING_PARAMS",void 0);if(!ai()&&
Pi)for(var oo=p(Object.keys(Pi)),oc=oo.next();!oc.done;oc=oo.next()){var po=oc.value,Qi=new Yg;D(Qi,1,po);Qi.setValue(String(Pi[po]));od(Nb,4,Yg,Qi)}var Ri=G.params;if(Ri){var qo=p(Object.keys(Ri));for(oc=qo.next();!oc.done;oc=qo.next()){var ro=oc.value,Si=new Yg;D(Si,1,"client."+ro);Si.setValue(String(Ri[ro]));od(Nb,4,Yg,Si)}}var so=$h("SERVER_NAME"),to=$h("SERVER_VERSION");if(so&&to){var Ti=new Yg;D(Ti,1,"server.name");Ti.setValue(so);od(Nb,4,Yg,Ti);var Ui=new Yg;D(Ui,1,"server.version");Ui.setValue(to);
od(Nb,4,Yg,Ui)}var Tf=new ch;I(Tf,1,Nb);I(Tf,2,Ni);I(Tf,3,nc);mo=Tf}var uo=mo;if(!uo)break a;var vo=new Dh;nd(vo,163,Eh,uo);Dp(vo,void 0)}else{if(Tp())var wo=void 0;else{var be={stackTrace:G.stack};G.fileName&&(be.filename=G.fileName);var tb=G.lineNumber&&G.lineNumber.split?G.lineNumber.split(":"):[];0!==tb.length&&(1!==tb.length||isNaN(Number(tb[0]))?2!==tb.length||isNaN(Number(tb[0]))||isNaN(Number(tb[1]))||(be.lineNumber=Number(tb[0]),be.columnNumber=Number(tb[1])):be.lineNumber=Number(tb[0]));
var Vi={level:"ERROR_LEVEL_UNKNOWN",message:G.message,errorClassName:G.name,sampleWeight:G.sampleWeight};"ERROR"===ae?Vi.level="ERROR_LEVEL_ERROR":"WARNING"===ae&&(Vi.level="ERROR_LEVEL_WARNNING");var Gt={isObfuscated:!0,browserStackInfo:be},ed={pageUrl:window.location.href,kvPairs:[]};A("FEXP_EXPERIMENTS")&&(ed.experimentIds=A("FEXP_EXPERIMENTS"));var Wi=A("LATEST_ECATCHER_SERVICE_TRACKING_PARAMS",void 0);if(!ai()&&Wi)for(var xo=p(Object.keys(Wi)),pc=xo.next();!pc.done;pc=xo.next()){var yo=pc.value;
ed.kvPairs.push({key:yo,value:String(Wi[yo])})}var Xi=G.params;if(Xi){var zo=p(Object.keys(Xi));for(pc=zo.next();!pc.done;pc=zo.next()){var Ao=pc.value;ed.kvPairs.push({key:"client."+Ao,value:String(Xi[Ao])})}}var Bo=$h("SERVER_NAME"),Co=$h("SERVER_VERSION");Bo&&Co&&(ed.kvPairs.push({key:"server.name",value:Bo}),ed.kvPairs.push({key:"server.version",value:Co}));wo={errorMetadata:ed,stackTrace:Gt,logMessage:Vi}}var Do=wo;if(!Do)break a;ok("clientError",Do)}("ERROR"===ae||M("errors_flush_gel_always_killswitch"))&&
Zo()}if(!M("suppress_error_204_logging")){var ce=G.params||{},Ob={urlParams:{a:"logerror",t:"jserror",type:G.name,msg:G.message.substr(0,250),line:G.lineNumber,level:h,"client.name":ce.name},postParams:{url:A("PAGE_NAME",window.location.href),file:G.fileName},method:"POST"};ce.version&&(Ob["client.version"]=ce.version);if(Ob.postParams){G.stack&&(Ob.postParams.stack=G.stack);for(var Eo=p(Object.keys(ce)),Yi=Eo.next();!Yi.done;Yi=Eo.next()){var Fo=Yi.value;Ob.postParams["client."+Fo]=ce[Fo]}var Zi=
A("LATEST_ECATCHER_SERVICE_TRACKING_PARAMS",void 0);if(Zi)for(var Go=p(Object.keys(Zi)),$i=Go.next();!$i.done;$i=Go.next()){var Ho=$i.value;Ob.postParams[Ho]=Zi[Ho]}var Io=A("SERVER_NAME",void 0),Jo=A("SERVER_VERSION",void 0);Io&&Jo&&(Ob.postParams["server.name"]=Io,Ob.postParams["server.version"]=Jo)}dj(A("ECATCHER_REPORT_HOST","")+"/error_204",Ob)}try{Lp.add(G.message)}catch(ev){}Mp++}}}}
function Tp(){for(var a=p(Qp),b=a.next();!b.done;b=a.next())if(fk(b.value.toLowerCase()))return!0;return!1}
function Up(a){var b=Ja.apply(1,arguments),c=a;c.args||(c.args=[]);c.args.push.apply(c.args,fa(b))}
;function Vp(){this.register=new Map}
function Wp(a){a=p(a.register.values());for(var b=a.next();!b.done;b=a.next())b.value.Mo("ABORTED")}
Vp.prototype.clear=function(){Wp(this);this.register.clear()};
var Xp=new Vp;var Yp=Date.now().toString();
function Zp(){a:{if(window.crypto&&window.crypto.getRandomValues)try{var a=Array(16),b=new Uint8Array(16);window.crypto.getRandomValues(b);for(var c=0;c<a.length;c++)a[c]=b[c];var d=a;break a}catch(e){}d=Array(16);for(a=0;16>a;a++){b=Date.now();for(c=0;c<b%23;c++)d[a]=Math.random();d[a]=Math.floor(256*Math.random())}if(Yp)for(a=1,b=0;b<Yp.length;b++)d[a%16]=d[a%16]^d[(a-1)%16]/4^Yp.charCodeAt(b),a++}a=[];for(b=0;b<d.length;b++)a.push("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_".charAt(d[b]&63));
return a.join("")}
;var $p=x.ytLoggingDocDocumentNonce_;$p||($p=Zp(),Za("ytLoggingDocDocumentNonce_",$p));var aq=$p;var bq={wg:0,qd:1,yd:2,Yj:3,yg:4,Yn:5,Ok:6,pm:7,Rl:8,0:"DEFAULT",1:"CHAT",2:"CONVERSATIONS",3:"MINIPLAYER",4:"DIALOG",5:"VOZ",6:"MUSIC_WATCH_TABS",7:"SHARE",8:"PUSH_NOTIFICATIONS"};function cq(a){this.h=a}
function dq(a){return new cq({trackingParams:a})}
cq.prototype.getAsJson=function(){var a={};void 0!==this.h.trackingParams?a.trackingParams=this.h.trackingParams:(a.veType=this.h.veType,void 0!==this.h.veCounter&&(a.veCounter=this.h.veCounter),void 0!==this.h.elementIndex&&(a.elementIndex=this.h.elementIndex));void 0!==this.h.dataElement&&(a.dataElement=this.h.dataElement.getAsJson());void 0!==this.h.youtubeData&&(a.youtubeData=this.h.youtubeData);return a};
cq.prototype.getAsJspb=function(){var a=new hh;void 0!==this.h.trackingParams?D(a,1,this.h.trackingParams):(void 0!==this.h.veType&&D(a,2,this.h.veType),void 0!==this.h.veCounter&&D(a,6,this.h.veCounter),void 0!==this.h.elementIndex&&D(a,3,this.h.elementIndex));if(void 0!==this.h.dataElement){var b=this.h.dataElement.getAsJspb();I(a,7,b)}void 0!==this.h.youtubeData&&I(a,8,this.h.jspbYoutubeData);return a};
cq.prototype.toString=function(){return JSON.stringify(this.getAsJson())};
cq.prototype.isClientVe=function(){return!this.h.trackingParams&&!!this.h.veType};function eq(a){a=void 0===a?0:a;return 0==a?"client-screen-nonce":"client-screen-nonce."+a}
function fq(a){a=void 0===a?0:a;return 0==a?"ROOT_VE_TYPE":"ROOT_VE_TYPE."+a}
function gq(a){return A(fq(void 0===a?0:a),void 0)}
y("yt_logging_screen.getRootVeType",gq,void 0);function hq(a){return(a=gq(void 0===a?0:a))?new cq({veType:a,youtubeData:void 0,jspbYoutubeData:void 0}):null}
function iq(){var a=A("csn-to-ctt-auth-info");a||(a={},Zh("csn-to-ctt-auth-info",a));return a}
function jq(a){a=A(eq(void 0===a?0:a));if(!a&&!A("USE_CSN_FALLBACK",!0))return null;a||(a="UNDEFINED_CSN");return a?a:null}
y("yt_logging_screen.getCurrentCsn",jq,void 0);function kq(a,b,c){var d=iq();(c=jq(c))&&delete d[c];b&&(d[a]=b)}
function lq(a){return iq()[a]}
y("yt_logging_screen.getCttAuthInfo",lq,void 0);
function mq(a,b,c,d){c=void 0===c?0:c;if(a!==A(eq(c))||b!==A(fq(c)))kq(a,d,c),Zh(eq(c),a),Zh(fq(c),b),b=function(){setTimeout(function(){if(a)if(M("web_time_via_jspb")){var e=new ih;D(e,1,aq);D(e,2,a);M("use_default_heartbeat_client")?Kp(e):Kp(e,void 0,sn)}else e={clientDocumentNonce:aq,clientScreenNonce:a},M("use_default_heartbeat_client")?ok("foregroundHeartbeatScreenAssociated",e):np("foregroundHeartbeatScreenAssociated",e,sn)},0)},"requestAnimationFrame"in window?window.requestAnimationFrame(b):
b()}
y("yt_logging_screen.setCurrentScreen",mq,void 0);var nq=window.yt&&window.yt.msgs_||window.ytcfg&&window.ytcfg.msgs||{};y("yt.msgs_",nq,void 0);function oq(a){Oh(nq,arguments)}
;var pq={pd:3611,Ec:27686,Fc:85013,Gc:23462,Ic:42016,Jc:62407,Kc:26926,Hc:43781,Lc:51236,Mc:79148,Nc:50160,Oc:77504,bd:87907,cd:18630,dd:54445,ed:80935,fd:105675,gd:37521,hd:47786,jd:98349,kd:123695,ld:6827,md:29434,nd:7282,od:124448,sd:32276,rd:76278,td:93911,ud:106531,vd:27259,wd:27262,xd:27263,zd:21759,Ad:27107,Bd:62936,Cd:49568,Dd:38408,Ed:80637,Fd:68727,Gd:68728,Hd:80353,Id:80356,Jd:74610,Kd:45707,Ld:83962,Md:83970,Nd:46713,Od:89711,Pd:74612,Qd:93265,Rd:74611,Sd:131380,Ud:128979,Vd:139311,Wd:128978,
Td:131391,Xd:105350,Zd:139312,ae:134800,Yd:131392,ce:113533,de:93252,ee:99357,he:94521,ie:114252,je:113532,ke:94522,ge:94583,le:88E3,me:139580,ne:93253,oe:93254,pe:94387,qe:94388,re:93255,se:97424,be:72502,te:110111,ue:76019,xe:117092,ye:117093,we:89431,ze:110466,Ae:77240,Be:60508,Ce:137401,De:137402,Ee:137046,Fe:73393,Ge:113534,He:92098,Ie:131381,Je:84517,Ke:83759,Le:80357,Me:86113,Ne:72598,Oe:72733,Pe:107349,Qe:124275,Re:118203,Se:133275,Te:133274,Ue:133272,Ve:133273,We:133276,Xe:144507,Ye:143247,
Ze:143248,af:143249,bf:143250,cf:143251,df:144401,ff:117431,ef:133797,gf:128572,hf:133405,jf:117429,kf:117430,lf:117432,mf:120080,nf:117259,pf:121692,qf:145656,rf:145655,sf:145653,tf:145654,uf:145657,vf:132972,wf:133051,xf:133658,yf:132971,zf:97615,Bf:143359,Af:143356,Df:143361,Cf:143358,Ff:143360,Ef:143357,Gf:142303,Hf:143353,If:143354,Jf:144479,Kf:143355,Lf:31402,Nf:133624,Of:146477,Pf:133623,Qf:133622,Mf:133621,Rf:84774,Sf:95117,Tf:98930,Uf:98931,Vf:98932,Wf:43347,Xf:129889,Yf:45474,Zf:100352,
ag:84758,cg:98443,dg:117985,eg:74613,fg:74614,gg:64502,hg:136032,ig:74615,jg:74616,kg:122224,lg:74617,mg:77820,ng:74618,og:93278,pg:93274,qg:93275,rg:93276,sg:22110,tg:29433,ug:133798,vg:132295,xg:120541,zg:82047,Ag:113550,Bg:75836,Cg:75837,Dg:42352,Eg:84512,Fg:76065,Gg:75989,Hg:16623,Ig:32594,Jg:27240,Kg:32633,Lg:74858,Ng:3945,Mg:16989,Og:45520,Pg:25488,Qg:25492,Rg:25494,Sg:55760,Tg:14057,Ug:18451,Vg:57204,Wg:57203,Xg:17897,Yg:57205,Zg:18198,ah:17898,bh:17909,dh:43980,eh:46220,fh:11721,gh:49954,
hh:96369,ih:3854,jh:56251,kh:25624,Bh:16906,Ch:99999,Dh:68172,Eh:27068,Fh:47973,Gh:72773,Hh:26970,Ih:26971,Jh:96805,Kh:17752,Lh:73233,Mh:109512,Nh:22256,Oh:14115,Ph:22696,Qh:89278,Rh:89277,Sh:109513,Th:43278,Uh:43459,Vh:43464,Wh:89279,Xh:43717,Yh:55764,Zh:22255,ai:89281,bi:40963,ci:43277,di:43442,fi:91824,gi:120137,hi:96367,ii:36850,ji:72694,ki:37414,li:36851,ni:124863,mi:121343,oi:73491,ri:54473,si:43375,ti:46674,vi:143815,wi:139095,xi:144402,yi:32473,zi:72901,Ai:72906,Bi:50947,Ci:50612,Di:50613,
Ei:50942,Fi:84938,Gi:84943,Hi:84939,Ii:84941,Ji:84944,Ki:84940,Li:84942,Mi:35585,Ni:51926,Oi:79983,Pi:63238,Qi:18921,Ri:63241,Si:57893,Ti:41182,Ui:135732,Vi:33424,Wi:22207,Xi:42993,Yi:36229,Zi:22206,aj:22205,bj:18993,cj:19001,dj:18990,ej:18991,fj:18997,gj:18725,hj:19003,ij:36874,jj:44763,kj:33427,lj:67793,mj:22182,nj:37091,oj:34650,pj:50617,qj:47261,rj:22287,sj:25144,tj:97917,uj:62397,vj:125598,wj:137935,xj:36961,yj:108035,zj:27426,Aj:27857,Bj:27846,Cj:27854,Dj:69692,Ej:61411,Fj:39299,Gj:38696,Hj:62520,
Ij:36382,Jj:108701,Kj:50663,Lj:36387,Mj:14908,Nj:37533,Oj:105443,Pj:61635,Qj:62274,Rj:133818,Sj:65702,Tj:65703,Uj:65701,Vj:76256,Wj:37671,Xj:49953,Zj:36216,ak:28237,bk:39553,ck:29222,dk:26107,ek:38050,fk:26108,hk:120745,gk:26109,ik:26110,jk:66881,kk:28236,lk:14586,mk:57929,nk:74723,pk:44098,qk:44099,tk:23528,uk:61699,rk:134104,sk:134103,vk:59149,wk:101951,xk:97346,yk:118051,zk:95102,Ak:64882,Bk:119505,Ck:63595,Dk:63349,Ek:95101,Fk:75240,Gk:27039,Hk:68823,Ik:21537,Jk:83464,Kk:75707,Lk:83113,Mk:101952,
Nk:101953,Pk:79610,Qk:125755,Rk:24402,Sk:24400,Tk:32925,Uk:57173,Vk:122502,Wk:145268,Xk:138480,Yk:64423,Zk:64424,al:33986,bl:100828,dl:129089,fl:21409,ll:135155,ml:135156,nl:135157,ol:135158,pl:135159,ql:135160,rl:135161,sl:135162,ul:135163,vl:135164,wl:135165,xl:135166,il:11070,jl:11074,kl:17880,yl:14001,Al:30709,Bl:30707,Cl:30711,Dl:30710,El:30708,zl:26984,Fl:146143,Gl:63648,Hl:63649,Il:51879,Jl:111059,Kl:5754,Ll:20445,Nl:130975,Ml:130976,Ol:110386,Pl:113746,Ql:66557,Sl:17310,Tl:28631,Ul:21589,
Vl:68012,Wl:60480,Xl:138664,Yl:141121,Zl:31571,am:141978,bm:76980,cm:41577,dm:45469,em:38669,fm:13768,gm:13777,hm:141842,im:62985,jm:4724,km:59369,lm:43927,mm:43928,nm:12924,om:100355,rm:56219,sm:27669,tm:10337,qm:47896,um:122629,wm:139723,vm:139722,xm:121258,ym:107598,zm:127991,Am:96639,Bm:107536,Cm:130169,Dm:96661,Em:145188,Fm:96658,Gm:116646,Hm:121122,Im:96660,Jm:127738,Km:127083,Lm:104443,Mm:96659,Nm:106442,Om:134840,Pm:63667,Qm:63668,Rm:63669,Sm:130686,Tm:147036,Um:78314,Vm:55761,Wm:127098,Xm:134841,
Ym:96368,Zm:67374,an:48992,bn:146176,cn:49956,dn:31961,en:26388,fn:23811,gn:5E4,hn:126250,jn:96370,kn:47355,ln:47356,mn:37935,nn:45521,pn:21760,qn:83769,rn:49977,sn:49974,tn:93497,un:93498,vn:34325,wn:140759,xn:115803,yn:123707,zn:100081,An:35309,Bn:68314,Cn:25602,Dn:100339,En:143516,Fn:59018,Gn:18248,Hn:50625,In:9729,Jn:37168,Kn:37169,Ln:21667,Mn:16749,Nn:18635,On:39305,Pn:18046,Qn:53969,Rn:8213,Sn:93926,Tn:102852,Un:110099,Vn:22678,Wn:69076,Xn:137575,Zn:139224,ao:100856,bo:17736,co:3832,eo:147111,
fo:55759,ho:64031,oo:93044,po:93045,qo:34388,ro:17657,so:17655,to:39579,uo:39578,vo:77448,wo:8196,xo:11357,yo:69877,zo:8197,Ao:82039};function qq(){var a=vb(rq),b;return gg(new $f(function(c,d){a.onSuccess=function(e){Bi(e)?c(new sq(e)):d(new tq("Request failed, status="+(e&&"status"in e?e.status:-1),"net.badstatus",e))};
a.onError=function(e){d(new tq("Unknown request error","net.unknown",e))};
a.onTimeout=function(e){d(new tq("Request timed out","net.timeout",e))};
b=dj("//googleads.g.doubleclick.net/pagead/id",a)}),function(c){c instanceof hg&&b.abort();
return eg(c)})}
function tq(a,b,c){bb.call(this,a+", errorCode="+b);this.errorCode=b;this.xhr=c;this.name="PromiseAjaxError"}
r(tq,bb);function sq(a){this.xhr=a}
;function uq(){this.i=0;this.h=null}
uq.prototype.then=function(a,b,c){return 1===this.i&&a?(a=a.call(c,this.h),Zf(a)?a:vq(a)):2===this.i&&b?(a=b.call(c,this.h),Zf(a)?a:wq(a)):this};
uq.prototype.getValue=function(){return this.h};
uq.prototype.$goog_Thenable=!0;function wq(a){var b=new uq;a=void 0===a?null:a;b.i=2;b.h=void 0===a?null:a;return b}
function vq(a){var b=new uq;a=void 0===a?null:a;b.i=1;b.h=void 0===a?null:a;return b}
;function xq(){if(le()||ek&&fk("applewebkit")&&!fk("version")&&(!fk("safari")||fk("gsa/"))||wc&&fk("version/"))return!0;if(M("enable_web_eom_visitor_data"))return A("EOM_VISITOR_DATA",void 0)?!1:!0;var a=A("INNERTUBE_CLIENT_NAME");return!a||"WEB"!==a&&"MWEB"!==a?!0:(a=pj("CONSENT"))?a.startsWith("YES+"):!0}
;function yq(a){bb.call(this,a.message||a.description||a.name);this.isMissing=a instanceof zq;this.isTimeout=a instanceof tq&&"net.timeout"==a.errorCode;this.isCanceled=a instanceof hg}
r(yq,bb);yq.prototype.name="BiscottiError";function zq(){bb.call(this,"Biscotti ID is missing from server")}
r(zq,bb);zq.prototype.name="BiscottiMissingError";var rq={format:"RAW",method:"GET",timeout:5E3,withCredentials:!0},Aq=null;function li(){if(M("disable_biscotti_fetch_entirely_for_all_web_clients"))return eg(Error("Biscotti id fetching has been disabled entirely."));if(!xq())return eg(Error("User has not consented - not fetching biscotti id."));if("1"==rb())return eg(Error("Biscotti ID is not available in private embed mode"));Aq||(Aq=gg(qq().then(Bq),function(a){return Cq(2,a)}));
return Aq}
function Bq(a){a=a.xhr.responseText;if(0!=a.lastIndexOf(")]}'",0))throw new zq;a=JSON.parse(a.substr(4));if(1<(a.type||1))throw new zq;a=a.id;mi(a);Aq=vq(a);Dq(18E5,2);return a}
function Cq(a,b){b=new yq(b);mi("");Aq=wq(b);0<a&&Dq(12E4,a-1);throw b;}
function Dq(a,b){Ci(function(){gg(qq().then(Bq,function(c){return Cq(b,c)}),Ma)},a)}
function Eq(){try{var a=z("yt.ads.biscotti.getId_");return a?a():li()}catch(b){return eg(b)}}
;function Fq(a){if("1"!=rb()){a&&ki();try{Eq().then(function(){},function(){}),Ci(Fq,18E5)}catch(b){ii(b)}}}
;function Gq(){this.zc=!0}
function Hq(a){var b={},c=ne([]);c&&(b.Authorization=c,c=a=null==a?void 0:a.sessionIndex,void 0===c&&(c=Number(A("SESSION_INDEX",0)),c=isNaN(c)?0:c),b["X-Goog-AuthUser"]=c,"INNERTUBE_HOST_OVERRIDE"in Sh||(b["X-Origin"]=window.location.origin),void 0===a&&"DELEGATED_SESSION_ID"in Sh&&(b["X-Goog-PageId"]=A("DELEGATED_SESSION_ID")));return b}
;var Iq=Symbol("injectionDeps");function Jq(a){this.name=a}
Jq.prototype.toString=function(){return"InjectionToken("+this.name+")"};
function Kq(){this.key=Lq}
function Mq(){this.providers=new Map;this.h=new Map}
Mq.prototype.resolve=function(a){return a instanceof Kq?Nq(this,a.key,[],!0):Nq(this,a,[])};
function Nq(a,b,c,d){d=void 0===d?!1:d;if(-1<c.indexOf(b))throw Error("Deps cycle for: "+b);if(a.h.has(b))return a.h.get(b);if(!a.providers.has(b)){if(d)return;throw Error("No provider for: "+b);}d=a.providers.get(b);c.push(b);if(d.Dc)var e=d.Dc;else if(d.Cc)e=d[Iq]?Oq(a,d[Iq],c):[],e=d.Cc.apply(d,fa(e));else if(d.Kb){e=d.Kb;var f=e[Iq]?Oq(a,e[Iq],c):[];e=new (Function.prototype.bind.apply(e,[null].concat(fa(f))))}else throw Error("Could not resolve providers for: "+b);c.pop();d.Qo||a.h.set(b,e);
return e}
function Oq(a,b,c){return b?b.map(function(d){return d instanceof Kq?Nq(a,d.key,c,!0):Nq(a,d,c)}):[]}
;var Pq;var Qq={identityType:"UNAUTHENTICATED_IDENTITY_TYPE_UNKNOWN"};var Rq=new Map([["dark","USER_INTERFACE_THEME_DARK"],["light","USER_INTERFACE_THEME_LIGHT"]]);function Sq(){var a=void 0===a?window.location.href:a;if(M("kevlar_disable_theme_param"))return null;bc(cc(5,a));try{var b=ui(a).theme;return Rq.get(b)||null}catch(c){}return null}
;function Tq(){this.h={};if(this.i=qj()){var a=pj("CONSISTENCY");a&&Uq(this,{encryptedTokenJarContents:a})}}
Tq.prototype.handleResponse=function(a,b){var c,d;b=(null==(c=b.ba.context)?void 0:null==(d=c.request)?void 0:d.consistencyTokenJars)||[];var e;(a=null==(e=a.responseContext)?void 0:e.consistencyTokenJar)&&this.replace(b,a)};
Tq.prototype.replace=function(a,b){a=p(a);for(var c=a.next();!c.done;c=a.next())delete this.h[c.value.encryptedTokenJarContents];Uq(this,b)};
function Uq(a,b){if(b.encryptedTokenJarContents&&(a.h[b.encryptedTokenJarContents]=b,"string"===typeof b.expirationSeconds)){var c=Number(b.expirationSeconds);setTimeout(function(){delete a.h[b.encryptedTokenJarContents]},1E3*c);
a.i&&oj("CONSISTENCY",b.encryptedTokenJarContents,c,void 0,!0)}}
;var Vq=window.location.hostname.split(".").slice(-2).join(".");function Wq(){var a=A("LOCATION_PLAYABILITY_TOKEN");"TVHTML5"===A("INNERTUBE_CLIENT_NAME")&&(this.h=Xq(this))&&(a=this.h.get("yt-location-playability-token"));a&&(this.locationPlayabilityToken=a,this.i=void 0)}
var Yq;Wq.getInstance=function(){Yq=z("yt.clientLocationService.instance");Yq||(Yq=new Wq,y("yt.clientLocationService.instance",Yq,void 0));return Yq};
Wq.prototype.setLocationOnInnerTubeContext=function(a){a.client||(a.client={});this.i?(a.client.locationInfo||(a.client.locationInfo={}),a.client.locationInfo.latitudeE7=1E7*this.i.coords.latitude,a.client.locationInfo.longitudeE7=1E7*this.i.coords.longitude,a.client.locationInfo.horizontalAccuracyMeters=Math.round(this.i.coords.accuracy),a.client.locationInfo.forceLocationPlayabilityTokenRefresh=!0):this.locationPlayabilityToken&&(a.client.locationPlayabilityToken=this.locationPlayabilityToken)};
Wq.prototype.handleResponse=function(a){var b;a=null==(b=a.responseContext)?void 0:b.locationPlayabilityToken;void 0!==a&&(this.locationPlayabilityToken=a,this.i=void 0,"TVHTML5"===A("INNERTUBE_CLIENT_NAME")?(this.h=Xq(this))&&this.h.set("yt-location-playability-token",a,15552E3):oj("YT_CL",JSON.stringify({loctok:a}),15552E3,Vq,!0))};
function Xq(a){return void 0===a.h?new Pj("yt-client-location"):a.h}
Wq.prototype.getCurrentPositionFromGeolocation=function(){var a=this;if(!(navigator&&navigator.geolocation&&navigator.geolocation.getCurrentPosition)||!M("web_enable_browser_geolocation_api")&&!M("enable_handoff_location_2fa_on_mweb"))return Promise.reject(Error("Geolocation unsupported"));var b=!1,c=1E4;M("enable_handoff_location_2fa_on_mweb")&&(b=!0,c=15E3);return new Promise(function(d,e){navigator.geolocation.getCurrentPosition(function(f){a.i=f;d(f)},function(f){e(f)},{enableHighAccuracy:b,
maximumAge:0,timeout:c})})};
Wq.prototype.createUnpluggedLocationInfo=function(a){var b={};a=a.coords;if(null==a?0:a.latitude)b.latitudeE7=Math.floor(1E7*a.latitude);if(null==a?0:a.longitude)b.longitudeE7=Math.floor(1E7*a.longitude);if(null==a?0:a.accuracy)b.locationRadiusMeters=Math.round(a.accuracy);return b};function Zq(a,b){var c;if((null==(c=a.signalServiceEndpoint)?0:c.signal)&&b.za&&(c=b.za[a.signalServiceEndpoint.signal]))return c();var d;if((null==(d=a.continuationCommand)?0:d.request)&&b.Rb&&(d=b.Rb[a.continuationCommand.request]))return d();for(var e in a)if(b.ob[e]&&(a=b.ob[e]))return a()}
;function $q(a){return function(){return new a}}
;var ar={},br=(ar.WEB_UNPLUGGED="^unplugged/",ar.WEB_UNPLUGGED_ONBOARDING="^unplugged/",ar.WEB_UNPLUGGED_OPS="^unplugged/",ar.WEB_UNPLUGGED_PUBLIC="^unplugged/",ar.WEB_CREATOR="^creator/",ar.WEB_KIDS="^kids/",ar.WEB_EXPERIMENTS="^experiments/",ar.WEB_MUSIC="^music/",ar.WEB_REMIX="^music/",ar.WEB_MUSIC_EMBEDDED_PLAYER="^music/",ar.WEB_MUSIC_EMBEDDED_PLAYER="^main_app/|^sfv/",ar);
function cr(a){var b=void 0===b?"UNKNOWN_INTERFACE":b;if(1===a.length)return a[0];var c=br[b];if(c){var d=new RegExp(c),e=p(a);for(c=e.next();!c.done;c=e.next())if(c=c.value,d.exec(c))return c}var f=[];Object.entries(br).forEach(function(g){var h=p(g);g=h.next().value;h=h.next().value;b!==g&&f.push(h)});
d=new RegExp(f.join("|"));a.sort(function(g,h){return g.length-h.length});
e=p(a);for(c=e.next();!c.done;c=e.next())if(c=c.value,!d.exec(c))return c;return a[0]}
;function dr(a,b){return{method:void 0===b?"POST":b,mode:wi(a)?"same-origin":"cors",credentials:wi(a)?"same-origin":"include"}}
;function er(){}
er.prototype.o=function(a,b,c){b=void 0===b?{}:b;c=void 0===c?Qq:c;var d=a.clickTrackingParams,e=this.l,f=!1;f=void 0===f?!1:f;e=void 0===e?!1:e;var g=A("INNERTUBE_CONTEXT");if(g){g=wb(g);M("web_no_tracking_params_in_shell_killswitch")||delete g.clickTracking;g.client||(g.client={});var h=g.client;"MWEB"===h.clientName&&(h.clientFormFactor=A("IS_TABLET")?"LARGE_FORM_FACTOR":"SMALL_FORM_FACTOR");h.screenWidthPoints=window.innerWidth;h.screenHeightPoints=window.innerHeight;h.screenPixelDensity=Math.round(window.devicePixelRatio||
1);h.screenDensityFloat=window.devicePixelRatio||1;h.utcOffsetMinutes=-Math.floor((new Date).getTimezoneOffset());var k=void 0===k?!1:k;sj.getInstance();var m="USER_INTERFACE_THEME_LIGHT";vj(165)?m="USER_INTERFACE_THEME_DARK":vj(174)?m="USER_INTERFACE_THEME_LIGHT":!M("kevlar_legacy_browsers")&&window.matchMedia&&window.matchMedia("(prefers-color-scheme)").matches&&window.matchMedia("(prefers-color-scheme: dark)").matches&&(m="USER_INTERFACE_THEME_DARK");k=k?m:Sq()||m;h.userInterfaceTheme=k;if(!f){if(k=
Cj())h.connectionType=k;M("web_log_effective_connection_type")&&(k=Dj())&&(g.client.effectiveConnectionType=k)}var q;if(M("web_log_memory_total_kbytes")&&(null==(q=x.navigator)?0:q.deviceMemory)){var u;q=null==(u=x.navigator)?void 0:u.deviceMemory;g.client.memoryTotalKbytes=""+1E6*q}u=ui(x.location.href);!M("web_populate_internal_geo_killswitch")&&u.internalcountrycode&&(h.internalGeo=u.internalcountrycode);"MWEB"===h.clientName||"WEB"===h.clientName?(h.mainAppWebInfo={graftUrl:x.location.href},M("kevlar_woffle")&&
lj.h&&(h.mainAppWebInfo.pwaInstallabilityStatus=lj.h.h?"PWA_INSTALLABILITY_STATUS_CAN_BE_INSTALLED":"PWA_INSTALLABILITY_STATUS_UNKNOWN"),h.mainAppWebInfo.webDisplayMode=mj(),h.mainAppWebInfo.isWebNativeShareAvailable=navigator&&void 0!==navigator.share):"TVHTML5"===h.clientName&&(!M("web_lr_app_quality_killswitch")&&(u=A("LIVING_ROOM_APP_QUALITY"))&&(h.tvAppInfo=Object.assign(h.tvAppInfo||{},{appQuality:u})),u=A("LIVING_ROOM_CERTIFICATION_SCOPE"))&&(h.tvAppInfo=Object.assign(h.tvAppInfo||{},{certificationScope:u}));
if(!M("web_populate_time_zone_itc_killswitch")){b:{if("undefined"!==typeof Intl)try{var C=(new Intl.DateTimeFormat).resolvedOptions().timeZone;break b}catch(Na){}C=void 0}C&&(h.timeZone=C)}(C=di())?h.experimentsToken=C:delete h.experimentsToken;C=ei();Tq.h||(Tq.h=new Tq);h=Tq.h.h;u=[];q=0;for(var v in h)u[q++]=h[v];g.request=Object.assign({},g.request,{internalExperimentFlags:C,consistencyTokenJars:u});!M("web_prequest_context_killswitch")&&(v=A("INNERTUBE_CONTEXT_PREQUEST_CONTEXT"))&&(g.request.externalPrequestContext=
v);C=sj.getInstance();v=vj(58);C=C.get("gsml","");g.user=Object.assign({},g.user);v&&(g.user.enableSafetyMode=v);C&&(g.user.lockedSafetyMode=!0);M("warm_op_csn_cleanup")?e&&(f=jq())&&(g.clientScreenNonce=f):!f&&(f=jq())&&(g.clientScreenNonce=f);d&&(g.clickTracking={clickTrackingParams:d});if(d=z("yt.mdx.remote.remoteClient_"))g.remoteClient=d;M("web_enable_client_location_service")&&Wq.getInstance().setLocationOnInnerTubeContext(g);try{var E=xi(void 0),F=E.bid;delete E.bid;g.adSignalsInfo={params:[],
bid:F};var H=p(Object.entries(E));for(var R=H.next();!R.done;R=H.next()){var O=p(R.value),S=O.next().value,ia=O.next().value;E=S;F=ia;d=void 0;null==(d=g.adSignalsInfo.params)||d.push({key:E,value:""+F})}}catch(Na){Rp(Na)}H=g}else Rp(Error("Error: No InnerTubeContext shell provided in ytconfig.")),H={};H={context:H};if(R=this.h(a)){this.i(H,R,b);var N;b="/youtubei/v1/"+cr(this.j());var Aa;(a=null==(N=a.commandMetadata)?void 0:null==(Aa=N.webCommandMetadata)?void 0:Aa.apiUrl)&&(b=a);N=b;(Aa=A("INNERTUBE_HOST_OVERRIDE"))&&
(N=String(Aa)+String(ec(N)));Aa={};Aa.key=A("INNERTUBE_API_KEY");M("json_condensed_response")&&(Aa.prettyPrint="false");N=vi(N,Aa||{},!1);N={input:N,ra:dr(N),ba:H,config:Object.assign({},void 0)};N.config.Ga?N.config.Ga.identity=c:N.config.Ga={identity:c};return N}Rp(new Q("Error: Failed to create Request from Command.",a))};
da.Object.defineProperties(er.prototype,{l:{configurable:!0,enumerable:!0,get:function(){return!1}}});function fr(){}
r(fr,er);fr.prototype.o=function(){return{input:"/getDatasyncIdsEndpoint",ra:dr("/getDatasyncIdsEndpoint","GET"),ba:{}}};
fr.prototype.j=function(){return[]};
fr.prototype.h=function(){};
fr.prototype.i=function(){};var gr={},hr=(gr.GET_DATASYNC_IDS=$q(fr),gr);function ir(a){var b=Ja.apply(1,arguments);if(!jr(a)||b.some(function(e){return!jr(e)}))throw Error("Only objects may be merged.");
var c=a;b=p(b);for(var d=b.next();!d.done;d=b.next())kr(c,d.value);return c}
function kr(a,b){for(var c in b)if(jr(b[c])){if(c in a&&!jr(a[c]))throw Error("Cannot merge an object into a non-object.");c in a||(a[c]={});kr(a[c],b[c])}else if(lr(b[c])){if(c in a&&!lr(a[c]))throw Error("Cannot merge an array into a non-array.");c in a||(a[c]=[]);mr(a[c],b[c])}else a[c]=b[c];return a}
function mr(a,b){b=p(b);for(var c=b.next();!c.done;c=b.next())c=c.value,jr(c)?a.push(kr({},c)):lr(c)?a.push(mr([],c)):a.push(c);return a}
function jr(a){return"object"===typeof a&&!Array.isArray(a)}
function lr(a){return"object"===typeof a&&Array.isArray(a)}
;function nr(a,b){Ul.call(this,1,arguments);this.timer=b}
r(nr,Ul);var or=new Vl("aft-recorded",nr);var pr=window;function qr(){this.timing={};this.clearResourceTimings=function(){};
this.webkitClearResourceTimings=function(){};
this.mozClearResourceTimings=function(){};
this.msClearResourceTimings=function(){};
this.oClearResourceTimings=function(){}}
var T=pr.performance||pr.mozPerformance||pr.msPerformance||pr.webkitPerformance||new qr;var rr=!1,sr={'script[name="scheduler/scheduler"]':"sj",'script[name="player/base"]':"pj",'link[rel="stylesheet"][name="www-player"]':"pc",'link[rel="stylesheet"][name="player/www-player"]':"pc",'script[name="desktop_polymer/desktop_polymer"]':"dpj",'link[rel="import"][name="desktop_polymer"]':"dph",'script[name="mobile-c3"]':"mcj",'link[rel="stylesheet"][name="mobile-c3"]':"mcc",'script[name="player-plasma-ias-phone/base"]':"mcppj",'script[name="player-plasma-ias-tablet/base"]':"mcptj",'link[rel="stylesheet"][name="mobile-polymer-player-ias"]':"mcpc",
'link[rel="stylesheet"][name="mobile-polymer-player-svg-ias"]':"mcpsc",'script[name="mobile_blazer_core_mod"]':"mbcj",'link[rel="stylesheet"][name="mobile_blazer_css"]':"mbc",'script[name="mobile_blazer_logged_in_users_mod"]':"mbliuj",'script[name="mobile_blazer_logged_out_users_mod"]':"mblouj",'script[name="mobile_blazer_noncore_mod"]':"mbnj","#player_css":"mbpc",'script[name="mobile_blazer_desktopplayer_mod"]':"mbpj",'link[rel="stylesheet"][name="mobile_blazer_tablet_css"]':"mbtc",'script[name="mobile_blazer_watch_mod"]':"mbwj"};
Xa(T.clearResourceTimings||T.webkitClearResourceTimings||T.mozClearResourceTimings||T.msClearResourceTimings||T.oClearResourceTimings||Ma,T);function tr(a){var b=ur(a);if(b.aft)return b.aft;a=A((a||"")+"TIMING_AFT_KEYS",["ol"]);for(var c=a.length,d=0;d<c;d++){var e=b[a[d]];if(e)return e}return NaN}
function vr(){var a;if(M("csi_use_performance_navigation_timing")){var b,c,d,e=null==T?void 0:null==(a=T.getEntriesByType)?void 0:null==(b=a.call(T,"navigation"))?void 0:null==(c=b[0])?void 0:null==(d=c.toJSON)?void 0:d.call(c);e?(e.requestStart=wr(e.requestStart),e.responseEnd=wr(e.responseEnd),e.redirectStart=wr(e.redirectStart),e.redirectEnd=wr(e.redirectEnd),e.domainLookupEnd=wr(e.domainLookupEnd),e.connectStart=wr(e.connectStart),e.connectEnd=wr(e.connectEnd),e.responseStart=wr(e.responseStart),
e.secureConnectionStart=wr(e.secureConnectionStart),e.domainLookupStart=wr(e.domainLookupStart),e.isPerformanceNavigationTiming=!0,a=e):a=T.timing}else a=T.timing;return a}
function xr(){return M("csi_use_time_origin")&&T.timeOrigin?Math.floor(T.timeOrigin):T.timing.navigationStart}
function wr(a){return Math.round(xr()+a)}
function yr(a){var b;(b=z("ytcsi."+(a||"")+"data_"))||(b={tick:{},info:{}},Za("ytcsi."+(a||"")+"data_",b));return b}
function zr(a){a=yr(a);a.info||(a.info={});return a.info}
function ur(a){a=yr(a);a.tick||(a.tick={});return a.tick}
function Ar(a){a=yr(a);if(a.gel){var b=a.gel;b.gelInfos||(b.gelInfos={});b.gelTicks||(b.gelTicks={})}else a.gel={gelTicks:{},gelInfos:{}};return a.gel}
function Br(a){a=Ar(a);a.gelInfos||(a.gelInfos={});return a.gelInfos}
function Cr(a){var b=yr(a).nonce;b||(b=Zp(),yr(a).nonce=b);return b}
function Dr(a){var b=ur(a||""),c=tr(a);c&&!rr&&($l(or,new nr(Math.round(c-b._start),a)),rr=!0)}
function Er(a,b){for(var c=p(Object.keys(b)),d=c.next();!d.done;d=c.next())if(d=d.value,!Object.keys(a).includes(d)||"object"===typeof b[d]&&!Er(a[d],b[d]))return!1;return!0}
;function Fr(){if(T.getEntriesByType){var a=T.getEntriesByType("paint");if(a=jb(a,function(b){return"first-paint"===b.name}))return wr(a.startTime)}a=T.timing;
return a.dc?Math.max(0,a.dc):0}
;function Gr(){var a=z("ytcsi.debug");a||(a=[],y("ytcsi.debug",a,void 0),y("ytcsi.reference",{},void 0));return a}
function Hr(a){a=a||"";var b=z("ytcsi.reference");b||(Gr(),b=z("ytcsi.reference"));if(b[a])return b[a];var c=Gr(),d={timerName:a,info:{},tick:{},span:{},jspbInfo:[]};c.push(d);return b[a]=d}
;var U={},Ir=(U.auto_search="LATENCY_ACTION_AUTO_SEARCH",U.ad_to_ad="LATENCY_ACTION_AD_TO_AD",U.ad_to_video="LATENCY_ACTION_AD_TO_VIDEO",U["analytics.explore"]="LATENCY_ACTION_CREATOR_ANALYTICS_EXPLORE",U.app_startup="LATENCY_ACTION_APP_STARTUP",U["artist.analytics"]="LATENCY_ACTION_CREATOR_ARTIST_ANALYTICS",U["artist.events"]="LATENCY_ACTION_CREATOR_ARTIST_CONCERTS",U["artist.presskit"]="LATENCY_ACTION_CREATOR_ARTIST_PROFILE",U.browse="LATENCY_ACTION_BROWSE",U.cast_splash="LATENCY_ACTION_CAST_SPLASH",
U.channels="LATENCY_ACTION_CHANNELS",U.creator_channel_dashboard="LATENCY_ACTION_CREATOR_CHANNEL_DASHBOARD",U["channel.analytics"]="LATENCY_ACTION_CREATOR_CHANNEL_ANALYTICS",U["channel.comments"]="LATENCY_ACTION_CREATOR_CHANNEL_COMMENTS",U["channel.content"]="LATENCY_ACTION_CREATOR_POST_LIST",U["channel.copyright"]="LATENCY_ACTION_CREATOR_CHANNEL_COPYRIGHT",U["channel.editing"]="LATENCY_ACTION_CREATOR_CHANNEL_EDITING",U["channel.monetization"]="LATENCY_ACTION_CREATOR_CHANNEL_MONETIZATION",U["channel.music"]=
"LATENCY_ACTION_CREATOR_CHANNEL_MUSIC",U["channel.playlists"]="LATENCY_ACTION_CREATOR_CHANNEL_PLAYLISTS",U["channel.translations"]="LATENCY_ACTION_CREATOR_CHANNEL_TRANSLATIONS",U["channel.videos"]="LATENCY_ACTION_CREATOR_CHANNEL_VIDEOS",U["channel.live_streaming"]="LATENCY_ACTION_CREATOR_LIVE_STREAMING",U.chips="LATENCY_ACTION_CHIPS",U["dialog.copyright_strikes"]="LATENCY_ACTION_CREATOR_DIALOG_COPYRIGHT_STRIKES",U["dialog.uploads"]="LATENCY_ACTION_CREATOR_DIALOG_UPLOADS",U.direct_playback="LATENCY_ACTION_DIRECT_PLAYBACK",
U.embed="LATENCY_ACTION_EMBED",U.entity_key_serialization_perf="LATENCY_ACTION_ENTITY_KEY_SERIALIZATION_PERF",U.entity_key_deserialization_perf="LATENCY_ACTION_ENTITY_KEY_DESERIALIZATION_PERF",U.explore="LATENCY_ACTION_EXPLORE",U.home="LATENCY_ACTION_HOME",U.library="LATENCY_ACTION_LIBRARY",U.live="LATENCY_ACTION_LIVE",U.live_pagination="LATENCY_ACTION_LIVE_PAGINATION",U.onboarding="LATENCY_ACTION_ONBOARDING",U.parent_profile_settings="LATENCY_ACTION_KIDS_PARENT_PROFILE_SETTINGS",U.parent_tools_collection=
"LATENCY_ACTION_PARENT_TOOLS_COLLECTION",U.parent_tools_dashboard="LATENCY_ACTION_PARENT_TOOLS_DASHBOARD",U.player_att="LATENCY_ACTION_PLAYER_ATTESTATION",U["post.comments"]="LATENCY_ACTION_CREATOR_POST_COMMENTS",U["post.edit"]="LATENCY_ACTION_CREATOR_POST_EDIT",U.prebuffer="LATENCY_ACTION_PREBUFFER",U.prefetch="LATENCY_ACTION_PREFETCH",U.profile_settings="LATENCY_ACTION_KIDS_PROFILE_SETTINGS",U.profile_switcher="LATENCY_ACTION_LOGIN",U.reel_watch="LATENCY_ACTION_REEL_WATCH",U.results="LATENCY_ACTION_RESULTS",
U.search_ui="LATENCY_ACTION_SEARCH_UI",U.search_suggest="LATENCY_ACTION_SUGGEST",U.search_zero_state="LATENCY_ACTION_SEARCH_ZERO_STATE",U.secret_code="LATENCY_ACTION_KIDS_SECRET_CODE",U.seek="LATENCY_ACTION_PLAYER_SEEK",U.settings="LATENCY_ACTION_SETTINGS",U.tenx="LATENCY_ACTION_TENX",U.video_to_ad="LATENCY_ACTION_VIDEO_TO_AD",U.watch="LATENCY_ACTION_WATCH",U.watch_it_again="LATENCY_ACTION_KIDS_WATCH_IT_AGAIN",U["watch,watch7"]="LATENCY_ACTION_WATCH",U["watch,watch7_html5"]="LATENCY_ACTION_WATCH",
U["watch,watch7ad"]="LATENCY_ACTION_WATCH",U["watch,watch7ad_html5"]="LATENCY_ACTION_WATCH",U.wn_comments="LATENCY_ACTION_LOAD_COMMENTS",U.ww_rqs="LATENCY_ACTION_WHO_IS_WATCHING",U["video.analytics"]="LATENCY_ACTION_CREATOR_VIDEO_ANALYTICS",U["video.comments"]="LATENCY_ACTION_CREATOR_VIDEO_COMMENTS",U["video.edit"]="LATENCY_ACTION_CREATOR_VIDEO_EDIT",U["video.editor"]="LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR",U["video.editor_async"]="LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR_ASYNC",U["video.live_settings"]=
"LATENCY_ACTION_CREATOR_VIDEO_LIVE_SETTINGS",U["video.live_streaming"]="LATENCY_ACTION_CREATOR_VIDEO_LIVE_STREAMING",U["video.monetization"]="LATENCY_ACTION_CREATOR_VIDEO_MONETIZATION",U["video.translations"]="LATENCY_ACTION_CREATOR_VIDEO_TRANSLATIONS",U.voice_assistant="LATENCY_ACTION_VOICE_ASSISTANT",U.cast_load_by_entity_to_watch="LATENCY_ACTION_CAST_LOAD_BY_ENTITY_TO_WATCH",U.networkless_performance="LATENCY_ACTION_NETWORKLESS_PERFORMANCE",U),V={},Jr=(V.ad_allowed="adTypesAllowed",V.yt_abt="adBreakType",
V.ad_cpn="adClientPlaybackNonce",V.ad_docid="adVideoId",V.yt_ad_an="adNetworks",V.ad_at="adType",V.aida="appInstallDataAgeMs",V.browse_id="browseId",V.p="httpProtocol",V.t="transportProtocol",V.cs="commandSource",V.cpn="clientPlaybackNonce",V.ccs="creatorInfo.creatorCanaryState",V.ctop="creatorInfo.topEntityType",V.csn="clientScreenNonce",V.docid="videoId",V.GetHome_rid="requestIds",V.GetSearch_rid="requestIds",V.GetPlayer_rid="requestIds",V.GetWatchNext_rid="requestIds",V.GetBrowse_rid="requestIds",
V.GetLibrary_rid="requestIds",V.is_continuation="isContinuation",V.is_nav="isNavigation",V.b_p="kabukiInfo.browseParams",V.is_prefetch="kabukiInfo.isPrefetch",V.is_secondary_nav="kabukiInfo.isSecondaryNav",V.nav_type="kabukiInfo.navigationType",V.prev_browse_id="kabukiInfo.prevBrowseId",V.query_source="kabukiInfo.querySource",V.voz_type="kabukiInfo.vozType",V.yt_lt="loadType",V.mver="creatorInfo.measurementVersion",V.yt_ad="isMonetized",V.nr="webInfo.navigationReason",V.nrsu="navigationRequestedSameUrl",
V.pnt="performanceNavigationTiming",V.prt="playbackRequiresTap",V.plt="playerInfo.playbackType",V.pis="playerInfo.playerInitializedState",V.paused="playerInfo.isPausedOnLoad",V.yt_pt="playerType",V.fmt="playerInfo.itag",V.yt_pl="watchInfo.isPlaylist",V.yt_pre="playerInfo.preloadType",V.yt_ad_pr="prerollAllowed",V.pa="previousAction",V.yt_red="isRedSubscriber",V.rce="mwebInfo.responseContentEncoding",V.rc="resourceInfo.resourceCache",V.scrh="screenHeight",V.scrw="screenWidth",V.st="serverTimeMs",V.ssdm=
"shellStartupDurationMs",V.br_trs="tvInfo.bedrockTriggerState",V.kebqat="kabukiInfo.earlyBrowseRequestInfo.abandonmentType",V.kebqa="kabukiInfo.earlyBrowseRequestInfo.adopted",V.label="tvInfo.label",V.is_mdx="tvInfo.isMdx",V.preloaded="tvInfo.isPreloaded",V.aac_type="tvInfo.authAccessCredentialType",V.upg_player_vis="playerInfo.visibilityState",V.query="unpluggedInfo.query",V.upg_chip_ids_string="unpluggedInfo.upgChipIdsString",V.yt_vst="videoStreamType",V.vph="viewportHeight",V.vpw="viewportWidth",
V.yt_vis="isVisible",V.rcl="mwebInfo.responseContentLength",V.GetSettings_rid="requestIds",V.GetTrending_rid="requestIds",V.GetMusicSearchSuggestions_rid="requestIds",V.REQUEST_ID="requestIds",V),Kr="isContinuation isNavigation kabukiInfo.earlyBrowseRequestInfo.adopted kabukiInfo.isPrefetch kabukiInfo.isSecondaryNav isMonetized navigationRequestedSameUrl performanceNavigationTiming playerInfo.isPausedOnLoad prerollAllowed isRedSubscriber tvInfo.isMdx tvInfo.isPreloaded isVisible watchInfo.isPlaylist playbackRequiresTap".split(" "),
Lr={},Mr=(Lr.ccs="CANARY_STATE_",Lr.mver="MEASUREMENT_VERSION_",Lr.pis="PLAYER_INITIALIZED_STATE_",Lr.yt_pt="LATENCY_PLAYER_",Lr.pa="LATENCY_ACTION_",Lr.ctop="TOP_ENTITY_TYPE_",Lr.yt_vst="VIDEO_STREAM_TYPE_",Lr),Nr="all_vc ap aq c cbr cbrand cbrver cmodel cos cosver cplatform ctheme cver ei l_an l_mm plid srt yt_fss yt_li vpst vpni2 vpil2 icrc icrt pa GetAccountOverview_rid GetHistory_rid cmt d_vpct d_vpnfi d_vpni nsru pc pfa pfeh pftr pnc prerender psc rc start tcrt tcrc ssr vpr vps yt_abt yt_fn yt_fs yt_pft yt_pre yt_pt yt_pvis ytu_pvis yt_ref yt_sts tds".split(" ");
function Or(a){return Ir[a]||"LATENCY_ACTION_UNKNOWN"}
function Pr(a,b,c){c=Ar(c);if(c.gelInfos)c.gelInfos[a]=!0;else{var d={};c.gelInfos=(d[a]=!0,d)}if(a.match("_rid")){var e=a.split("_rid")[0];a="REQUEST_ID"}if(a in Jr){c=Jr[a];0<=eb(Kr,c)&&(b=!!b);a in Mr&&"string"===typeof b&&(b=Mr[a]+b.toUpperCase());a=b;b=c.split(".");for(var f=d={},g=0;g<b.length-1;g++){var h=b[g];f[h]={};f=f[h]}f[b[b.length-1]]="requestIds"===c?[{id:a,endpoint:e}]:a;return ir({},d)}0<=eb(Nr,a)||Sp(new Q("Unknown label logged with GEL CSI",a))}
;var W={LATENCY_ACTION_KIDS_PROFILE_SWITCHER:90,LATENCY_ACTION_OFFLINE_THUMBNAIL_TRANSFER:100,LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR_ASYNC:46,LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR:37,LATENCY_ACTION_SPINNER_DISPLAYED:14,LATENCY_ACTION_PLAYABILITY_CHECK:10,LATENCY_ACTION_PROCESS:9,LATENCY_ACTION_APP_STARTUP:5,LATENCY_ACTION_ABANDONED_DIRECT_PLAYBACK:154,LATENCY_ACTION_SHARE_VIDEO:153,LATENCY_ACTION_AD_TO_VIDEO_INT:152,LATENCY_ACTION_ABANDONED_BROWSE:151,LATENCY_ACTION_PLAYER_ROTATION:150,LATENCY_ACTION_SHOPPING_IN_APP:124,
LATENCY_ACTION_PLAYER_ATTESTATION:121,LATENCY_ACTION_PLAYER_SEEK:119,LATENCY_ACTION_SUPER_STICKER_BUY_FLOW:114,LATENCY_ACTION_BLOCKS_PERFORMANCE:148,LATENCY_ACTION_ASSISTANT_QUERY:138,LATENCY_ACTION_ASSISTANT_SETTINGS:137,LATENCY_ACTION_ENTITY_KEY_DESERIALIZATION_PERF:129,LATENCY_ACTION_ENTITY_KEY_SERIALIZATION_PERF:128,LATENCY_ACTION_PROOF_OF_ORIGIN_TOKEN_CREATE:127,LATENCY_ACTION_EMBEDS_SDK_INITIALIZATION:123,LATENCY_ACTION_NETWORKLESS_PERFORMANCE:122,LATENCY_ACTION_DOWNLOADS_EXPANSION:133,LATENCY_ACTION_ENTITY_TRANSFORM:131,
LATENCY_ACTION_DOWNLOADS_COMPATIBILITY_LAYER:96,LATENCY_ACTION_EMBEDS_SET_VIDEO:95,LATENCY_ACTION_SETTINGS:93,LATENCY_ACTION_ABANDONED_STARTUP:81,LATENCY_ACTION_MEDIA_BROWSER_ALARM_PLAY:80,LATENCY_ACTION_MEDIA_BROWSER_SEARCH:79,LATENCY_ACTION_MEDIA_BROWSER_LOAD_TREE:78,LATENCY_ACTION_WHO_IS_WATCHING:77,LATENCY_ACTION_CAST_LOAD_BY_ENTITY_TO_WATCH:76,LATENCY_ACTION_LITE_SWITCH_ACCOUNT:73,LATENCY_ACTION_ELEMENTS_PERFORMANCE:70,LATENCY_ACTION_LOCATION_SIGNAL_COLLECTION:69,LATENCY_ACTION_MODIFY_CHANNEL_NOTIFICATION:65,
LATENCY_ACTION_OFFLINE_STORE_START:61,LATENCY_ACTION_REEL_EDITOR:58,LATENCY_ACTION_CHANNEL_SUBSCRIBE:56,LATENCY_ACTION_CHANNEL_PREVIEW:55,LATENCY_ACTION_PREFETCH:52,LATENCY_ACTION_ABANDONED_WATCH:45,LATENCY_ACTION_LOAD_COMMENT_REPLIES:26,LATENCY_ACTION_LOAD_COMMENTS:25,LATENCY_ACTION_EDIT_COMMENT:24,LATENCY_ACTION_NEW_COMMENT:23,LATENCY_ACTION_OFFLINE_SHARING_RECEIVER_PAIRING:19,LATENCY_ACTION_EMBED:18,LATENCY_ACTION_MDX_LAUNCH:15,LATENCY_ACTION_RESOLVE_URL:13,LATENCY_ACTION_CAST_SPLASH:149,LATENCY_ACTION_MDX_CAST:120,
LATENCY_ACTION_MDX_COMMAND:12,LATENCY_ACTION_REEL_SELECT_SEGMENT:136,LATENCY_ACTION_ACCELERATED_EFFECTS:145,LATENCY_ACTION_UPLOAD_AUDIO_MIXER:147,LATENCY_ACTION_SHORTS_SEG_IMP_TRANSCODING:146,LATENCY_ACTION_SHORTS_AUDIO_PICKER_PLAYBACK:130,LATENCY_ACTION_SHORTS_WAVEFORM_DOWNLOAD:125,LATENCY_ACTION_SHORTS_VIDEO_INGESTION:155,LATENCY_ACTION_SHORTS_GALLERY:107,LATENCY_ACTION_SHORTS_TRIM:105,LATENCY_ACTION_SHORTS_EDIT:104,LATENCY_ACTION_SHORTS_CAMERA:103,LATENCY_ACTION_PARENT_TOOLS_DASHBOARD:102,LATENCY_ACTION_PARENT_TOOLS_COLLECTION:101,
LATENCY_ACTION_MUSIC_LOAD_RECOMMENDED_MEDIA_ITEMS:116,LATENCY_ACTION_MUSIC_LOAD_MEDIA_ITEMS:115,LATENCY_ACTION_MUSIC_ALBUM_DETAIL:72,LATENCY_ACTION_MUSIC_PLAYLIST_DETAIL:71,LATENCY_ACTION_CHIPS:68,LATENCY_ACTION_SEARCH_ZERO_STATE:67,LATENCY_ACTION_LIVE_PAGINATION:117,LATENCY_ACTION_LIVE:20,LATENCY_ACTION_PREBUFFER:40,LATENCY_ACTION_TENX:39,LATENCY_ACTION_KIDS_PROFILE_SETTINGS:94,LATENCY_ACTION_KIDS_WATCH_IT_AGAIN:92,LATENCY_ACTION_KIDS_SECRET_CODE:91,LATENCY_ACTION_KIDS_PARENT_PROFILE_SETTINGS:89,
LATENCY_ACTION_KIDS_ONBOARDING:88,LATENCY_ACTION_KIDS_VOICE_SEARCH:82,LATENCY_ACTION_KIDS_CURATED_COLLECTION:62,LATENCY_ACTION_KIDS_LIBRARY:53,LATENCY_ACTION_CREATOR_VIDEO_TRANSLATIONS:38,LATENCY_ACTION_CREATOR_VIDEO_MONETIZATION:74,LATENCY_ACTION_CREATOR_VIDEO_LIVE_STREAMING:141,LATENCY_ACTION_CREATOR_VIDEO_LIVE_SETTINGS:142,LATENCY_ACTION_CREATOR_VIDEO_EDITOR_ASYNC:51,LATENCY_ACTION_CREATOR_VIDEO_EDITOR:50,LATENCY_ACTION_CREATOR_VIDEO_EDIT:36,LATENCY_ACTION_CREATOR_VIDEO_COMMENTS:34,LATENCY_ACTION_CREATOR_VIDEO_ANALYTICS:33,
LATENCY_ACTION_CREATOR_POST_LIST:112,LATENCY_ACTION_CREATOR_POST_EDIT:110,LATENCY_ACTION_CREATOR_POST_COMMENTS:111,LATENCY_ACTION_CREATOR_LIVE_STREAMING:108,LATENCY_ACTION_CREATOR_DIALOG_UPLOADS:86,LATENCY_ACTION_CREATOR_DIALOG_COPYRIGHT_STRIKES:87,LATENCY_ACTION_CREATOR_CHANNEL_VIDEOS:32,LATENCY_ACTION_CREATOR_CHANNEL_TRANSLATIONS:48,LATENCY_ACTION_CREATOR_CHANNEL_PLAYLISTS:139,LATENCY_ACTION_CREATOR_CHANNEL_MUSIC:99,LATENCY_ACTION_CREATOR_CHANNEL_MONETIZATION:43,LATENCY_ACTION_CREATOR_CHANNEL_EDITING:113,
LATENCY_ACTION_CREATOR_CHANNEL_DASHBOARD:49,LATENCY_ACTION_CREATOR_CHANNEL_COPYRIGHT:44,LATENCY_ACTION_CREATOR_CHANNEL_COMMENTS:66,LATENCY_ACTION_CREATOR_CHANNEL_ANALYTICS:31,LATENCY_ACTION_CREATOR_ARTIST_PROFILE:85,LATENCY_ACTION_CREATOR_ARTIST_CONCERTS:84,LATENCY_ACTION_CREATOR_ARTIST_ANALYTICS:83,LATENCY_ACTION_CREATOR_ANALYTICS_EXPLORE:140,LATENCY_ACTION_STORYBOARD_THUMBNAILS:118,LATENCY_ACTION_SEARCH_THUMBNAILS:59,LATENCY_ACTION_ON_DEVICE_MODEL_DOWNLOAD:54,LATENCY_ACTION_VOICE_ASSISTANT:47,LATENCY_ACTION_SEARCH_UI:35,
LATENCY_ACTION_SUGGEST:30,LATENCY_ACTION_AUTO_SEARCH:126,LATENCY_ACTION_DOWNLOADS:98,LATENCY_ACTION_EXPLORE:75,LATENCY_ACTION_VIDEO_LIST:63,LATENCY_ACTION_HOME_RESUME:60,LATENCY_ACTION_SUBSCRIPTIONS_LIST:57,LATENCY_ACTION_THUMBNAIL_LOAD:42,LATENCY_ACTION_FIRST_THUMBNAIL_LOAD:29,LATENCY_ACTION_SUBSCRIPTIONS_FEED:109,LATENCY_ACTION_SUBSCRIPTIONS:28,LATENCY_ACTION_TRENDING:27,LATENCY_ACTION_LIBRARY:21,LATENCY_ACTION_VIDEO_THUMBNAIL:8,LATENCY_ACTION_SHOW_MORE:7,LATENCY_ACTION_VIDEO_PREVIEW:6,LATENCY_ACTION_PREBUFFER_VIDEO:144,
LATENCY_ACTION_PREFETCH_VIDEO:143,LATENCY_ACTION_DIRECT_PLAYBACK:132,LATENCY_ACTION_REEL_WATCH:41,LATENCY_ACTION_AD_TO_AD:22,LATENCY_ACTION_VIDEO_TO_AD:17,LATENCY_ACTION_AD_TO_VIDEO:16,LATENCY_ACTION_ONBOARDING:135,LATENCY_ACTION_LOGIN:97,LATENCY_ACTION_BROWSE:11,LATENCY_ACTION_CHANNELS:4,LATENCY_ACTION_WATCH:3,LATENCY_ACTION_RESULTS:2,LATENCY_ACTION_HOME:1,LATENCY_ACTION_STARTUP:106,LATENCY_ACTION_UNKNOWN:0};W[W.LATENCY_ACTION_KIDS_PROFILE_SWITCHER]="LATENCY_ACTION_KIDS_PROFILE_SWITCHER";
W[W.LATENCY_ACTION_OFFLINE_THUMBNAIL_TRANSFER]="LATENCY_ACTION_OFFLINE_THUMBNAIL_TRANSFER";W[W.LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR_ASYNC]="LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR_ASYNC";W[W.LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR]="LATENCY_ACTION_CREATOR_VIDEO_VIDEO_EDITOR";W[W.LATENCY_ACTION_SPINNER_DISPLAYED]="LATENCY_ACTION_SPINNER_DISPLAYED";W[W.LATENCY_ACTION_PLAYABILITY_CHECK]="LATENCY_ACTION_PLAYABILITY_CHECK";W[W.LATENCY_ACTION_PROCESS]="LATENCY_ACTION_PROCESS";
W[W.LATENCY_ACTION_APP_STARTUP]="LATENCY_ACTION_APP_STARTUP";W[W.LATENCY_ACTION_ABANDONED_DIRECT_PLAYBACK]="LATENCY_ACTION_ABANDONED_DIRECT_PLAYBACK";W[W.LATENCY_ACTION_SHARE_VIDEO]="LATENCY_ACTION_SHARE_VIDEO";W[W.LATENCY_ACTION_AD_TO_VIDEO_INT]="LATENCY_ACTION_AD_TO_VIDEO_INT";W[W.LATENCY_ACTION_ABANDONED_BROWSE]="LATENCY_ACTION_ABANDONED_BROWSE";W[W.LATENCY_ACTION_PLAYER_ROTATION]="LATENCY_ACTION_PLAYER_ROTATION";W[W.LATENCY_ACTION_SHOPPING_IN_APP]="LATENCY_ACTION_SHOPPING_IN_APP";
W[W.LATENCY_ACTION_PLAYER_ATTESTATION]="LATENCY_ACTION_PLAYER_ATTESTATION";W[W.LATENCY_ACTION_PLAYER_SEEK]="LATENCY_ACTION_PLAYER_SEEK";W[W.LATENCY_ACTION_SUPER_STICKER_BUY_FLOW]="LATENCY_ACTION_SUPER_STICKER_BUY_FLOW";W[W.LATENCY_ACTION_BLOCKS_PERFORMANCE]="LATENCY_ACTION_BLOCKS_PERFORMANCE";W[W.LATENCY_ACTION_ASSISTANT_QUERY]="LATENCY_ACTION_ASSISTANT_QUERY";W[W.LATENCY_ACTION_ASSISTANT_SETTINGS]="LATENCY_ACTION_ASSISTANT_SETTINGS";W[W.LATENCY_ACTION_ENTITY_KEY_DESERIALIZATION_PERF]="LATENCY_ACTION_ENTITY_KEY_DESERIALIZATION_PERF";
W[W.LATENCY_ACTION_ENTITY_KEY_SERIALIZATION_PERF]="LATENCY_ACTION_ENTITY_KEY_SERIALIZATION_PERF";W[W.LATENCY_ACTION_PROOF_OF_ORIGIN_TOKEN_CREATE]="LATENCY_ACTION_PROOF_OF_ORIGIN_TOKEN_CREATE";W[W.LATENCY_ACTION_EMBEDS_SDK_INITIALIZATION]="LATENCY_ACTION_EMBEDS_SDK_INITIALIZATION";W[W.LATENCY_ACTION_NETWORKLESS_PERFORMANCE]="LATENCY_ACTION_NETWORKLESS_PERFORMANCE";W[W.LATENCY_ACTION_DOWNLOADS_EXPANSION]="LATENCY_ACTION_DOWNLOADS_EXPANSION";W[W.LATENCY_ACTION_ENTITY_TRANSFORM]="LATENCY_ACTION_ENTITY_TRANSFORM";
W[W.LATENCY_ACTION_DOWNLOADS_COMPATIBILITY_LAYER]="LATENCY_ACTION_DOWNLOADS_COMPATIBILITY_LAYER";W[W.LATENCY_ACTION_EMBEDS_SET_VIDEO]="LATENCY_ACTION_EMBEDS_SET_VIDEO";W[W.LATENCY_ACTION_SETTINGS]="LATENCY_ACTION_SETTINGS";W[W.LATENCY_ACTION_ABANDONED_STARTUP]="LATENCY_ACTION_ABANDONED_STARTUP";W[W.LATENCY_ACTION_MEDIA_BROWSER_ALARM_PLAY]="LATENCY_ACTION_MEDIA_BROWSER_ALARM_PLAY";W[W.LATENCY_ACTION_MEDIA_BROWSER_SEARCH]="LATENCY_ACTION_MEDIA_BROWSER_SEARCH";
W[W.LATENCY_ACTION_MEDIA_BROWSER_LOAD_TREE]="LATENCY_ACTION_MEDIA_BROWSER_LOAD_TREE";W[W.LATENCY_ACTION_WHO_IS_WATCHING]="LATENCY_ACTION_WHO_IS_WATCHING";W[W.LATENCY_ACTION_CAST_LOAD_BY_ENTITY_TO_WATCH]="LATENCY_ACTION_CAST_LOAD_BY_ENTITY_TO_WATCH";W[W.LATENCY_ACTION_LITE_SWITCH_ACCOUNT]="LATENCY_ACTION_LITE_SWITCH_ACCOUNT";W[W.LATENCY_ACTION_ELEMENTS_PERFORMANCE]="LATENCY_ACTION_ELEMENTS_PERFORMANCE";W[W.LATENCY_ACTION_LOCATION_SIGNAL_COLLECTION]="LATENCY_ACTION_LOCATION_SIGNAL_COLLECTION";
W[W.LATENCY_ACTION_MODIFY_CHANNEL_NOTIFICATION]="LATENCY_ACTION_MODIFY_CHANNEL_NOTIFICATION";W[W.LATENCY_ACTION_OFFLINE_STORE_START]="LATENCY_ACTION_OFFLINE_STORE_START";W[W.LATENCY_ACTION_REEL_EDITOR]="LATENCY_ACTION_REEL_EDITOR";W[W.LATENCY_ACTION_CHANNEL_SUBSCRIBE]="LATENCY_ACTION_CHANNEL_SUBSCRIBE";W[W.LATENCY_ACTION_CHANNEL_PREVIEW]="LATENCY_ACTION_CHANNEL_PREVIEW";W[W.LATENCY_ACTION_PREFETCH]="LATENCY_ACTION_PREFETCH";W[W.LATENCY_ACTION_ABANDONED_WATCH]="LATENCY_ACTION_ABANDONED_WATCH";
W[W.LATENCY_ACTION_LOAD_COMMENT_REPLIES]="LATENCY_ACTION_LOAD_COMMENT_REPLIES";W[W.LATENCY_ACTION_LOAD_COMMENTS]="LATENCY_ACTION_LOAD_COMMENTS";W[W.LATENCY_ACTION_EDIT_COMMENT]="LATENCY_ACTION_EDIT_COMMENT";W[W.LATENCY_ACTION_NEW_COMMENT]="LATENCY_ACTION_NEW_COMMENT";W[W.LATENCY_ACTION_OFFLINE_SHARING_RECEIVER_PAIRING]="LATENCY_ACTION_OFFLINE_SHARING_RECEIVER_PAIRING";W[W.LATENCY_ACTION_EMBED]="LATENCY_ACTION_EMBED";W[W.LATENCY_ACTION_MDX_LAUNCH]="LATENCY_ACTION_MDX_LAUNCH";
W[W.LATENCY_ACTION_RESOLVE_URL]="LATENCY_ACTION_RESOLVE_URL";W[W.LATENCY_ACTION_CAST_SPLASH]="LATENCY_ACTION_CAST_SPLASH";W[W.LATENCY_ACTION_MDX_CAST]="LATENCY_ACTION_MDX_CAST";W[W.LATENCY_ACTION_MDX_COMMAND]="LATENCY_ACTION_MDX_COMMAND";W[W.LATENCY_ACTION_REEL_SELECT_SEGMENT]="LATENCY_ACTION_REEL_SELECT_SEGMENT";W[W.LATENCY_ACTION_ACCELERATED_EFFECTS]="LATENCY_ACTION_ACCELERATED_EFFECTS";W[W.LATENCY_ACTION_UPLOAD_AUDIO_MIXER]="LATENCY_ACTION_UPLOAD_AUDIO_MIXER";
W[W.LATENCY_ACTION_SHORTS_SEG_IMP_TRANSCODING]="LATENCY_ACTION_SHORTS_SEG_IMP_TRANSCODING";W[W.LATENCY_ACTION_SHORTS_AUDIO_PICKER_PLAYBACK]="LATENCY_ACTION_SHORTS_AUDIO_PICKER_PLAYBACK";W[W.LATENCY_ACTION_SHORTS_WAVEFORM_DOWNLOAD]="LATENCY_ACTION_SHORTS_WAVEFORM_DOWNLOAD";W[W.LATENCY_ACTION_SHORTS_VIDEO_INGESTION]="LATENCY_ACTION_SHORTS_VIDEO_INGESTION";W[W.LATENCY_ACTION_SHORTS_GALLERY]="LATENCY_ACTION_SHORTS_GALLERY";W[W.LATENCY_ACTION_SHORTS_TRIM]="LATENCY_ACTION_SHORTS_TRIM";
W[W.LATENCY_ACTION_SHORTS_EDIT]="LATENCY_ACTION_SHORTS_EDIT";W[W.LATENCY_ACTION_SHORTS_CAMERA]="LATENCY_ACTION_SHORTS_CAMERA";W[W.LATENCY_ACTION_PARENT_TOOLS_DASHBOARD]="LATENCY_ACTION_PARENT_TOOLS_DASHBOARD";W[W.LATENCY_ACTION_PARENT_TOOLS_COLLECTION]="LATENCY_ACTION_PARENT_TOOLS_COLLECTION";W[W.LATENCY_ACTION_MUSIC_LOAD_RECOMMENDED_MEDIA_ITEMS]="LATENCY_ACTION_MUSIC_LOAD_RECOMMENDED_MEDIA_ITEMS";W[W.LATENCY_ACTION_MUSIC_LOAD_MEDIA_ITEMS]="LATENCY_ACTION_MUSIC_LOAD_MEDIA_ITEMS";
W[W.LATENCY_ACTION_MUSIC_ALBUM_DETAIL]="LATENCY_ACTION_MUSIC_ALBUM_DETAIL";W[W.LATENCY_ACTION_MUSIC_PLAYLIST_DETAIL]="LATENCY_ACTION_MUSIC_PLAYLIST_DETAIL";W[W.LATENCY_ACTION_CHIPS]="LATENCY_ACTION_CHIPS";W[W.LATENCY_ACTION_SEARCH_ZERO_STATE]="LATENCY_ACTION_SEARCH_ZERO_STATE";W[W.LATENCY_ACTION_LIVE_PAGINATION]="LATENCY_ACTION_LIVE_PAGINATION";W[W.LATENCY_ACTION_LIVE]="LATENCY_ACTION_LIVE";W[W.LATENCY_ACTION_PREBUFFER]="LATENCY_ACTION_PREBUFFER";W[W.LATENCY_ACTION_TENX]="LATENCY_ACTION_TENX";
W[W.LATENCY_ACTION_KIDS_PROFILE_SETTINGS]="LATENCY_ACTION_KIDS_PROFILE_SETTINGS";W[W.LATENCY_ACTION_KIDS_WATCH_IT_AGAIN]="LATENCY_ACTION_KIDS_WATCH_IT_AGAIN";W[W.LATENCY_ACTION_KIDS_SECRET_CODE]="LATENCY_ACTION_KIDS_SECRET_CODE";W[W.LATENCY_ACTION_KIDS_PARENT_PROFILE_SETTINGS]="LATENCY_ACTION_KIDS_PARENT_PROFILE_SETTINGS";W[W.LATENCY_ACTION_KIDS_ONBOARDING]="LATENCY_ACTION_KIDS_ONBOARDING";W[W.LATENCY_ACTION_KIDS_VOICE_SEARCH]="LATENCY_ACTION_KIDS_VOICE_SEARCH";
W[W.LATENCY_ACTION_KIDS_CURATED_COLLECTION]="LATENCY_ACTION_KIDS_CURATED_COLLECTION";W[W.LATENCY_ACTION_KIDS_LIBRARY]="LATENCY_ACTION_KIDS_LIBRARY";W[W.LATENCY_ACTION_CREATOR_VIDEO_TRANSLATIONS]="LATENCY_ACTION_CREATOR_VIDEO_TRANSLATIONS";W[W.LATENCY_ACTION_CREATOR_VIDEO_MONETIZATION]="LATENCY_ACTION_CREATOR_VIDEO_MONETIZATION";W[W.LATENCY_ACTION_CREATOR_VIDEO_LIVE_STREAMING]="LATENCY_ACTION_CREATOR_VIDEO_LIVE_STREAMING";W[W.LATENCY_ACTION_CREATOR_VIDEO_LIVE_SETTINGS]="LATENCY_ACTION_CREATOR_VIDEO_LIVE_SETTINGS";
W[W.LATENCY_ACTION_CREATOR_VIDEO_EDITOR_ASYNC]="LATENCY_ACTION_CREATOR_VIDEO_EDITOR_ASYNC";W[W.LATENCY_ACTION_CREATOR_VIDEO_EDITOR]="LATENCY_ACTION_CREATOR_VIDEO_EDITOR";W[W.LATENCY_ACTION_CREATOR_VIDEO_EDIT]="LATENCY_ACTION_CREATOR_VIDEO_EDIT";W[W.LATENCY_ACTION_CREATOR_VIDEO_COMMENTS]="LATENCY_ACTION_CREATOR_VIDEO_COMMENTS";W[W.LATENCY_ACTION_CREATOR_VIDEO_ANALYTICS]="LATENCY_ACTION_CREATOR_VIDEO_ANALYTICS";W[W.LATENCY_ACTION_CREATOR_POST_LIST]="LATENCY_ACTION_CREATOR_POST_LIST";
W[W.LATENCY_ACTION_CREATOR_POST_EDIT]="LATENCY_ACTION_CREATOR_POST_EDIT";W[W.LATENCY_ACTION_CREATOR_POST_COMMENTS]="LATENCY_ACTION_CREATOR_POST_COMMENTS";W[W.LATENCY_ACTION_CREATOR_LIVE_STREAMING]="LATENCY_ACTION_CREATOR_LIVE_STREAMING";W[W.LATENCY_ACTION_CREATOR_DIALOG_UPLOADS]="LATENCY_ACTION_CREATOR_DIALOG_UPLOADS";W[W.LATENCY_ACTION_CREATOR_DIALOG_COPYRIGHT_STRIKES]="LATENCY_ACTION_CREATOR_DIALOG_COPYRIGHT_STRIKES";W[W.LATENCY_ACTION_CREATOR_CHANNEL_VIDEOS]="LATENCY_ACTION_CREATOR_CHANNEL_VIDEOS";
W[W.LATENCY_ACTION_CREATOR_CHANNEL_TRANSLATIONS]="LATENCY_ACTION_CREATOR_CHANNEL_TRANSLATIONS";W[W.LATENCY_ACTION_CREATOR_CHANNEL_PLAYLISTS]="LATENCY_ACTION_CREATOR_CHANNEL_PLAYLISTS";W[W.LATENCY_ACTION_CREATOR_CHANNEL_MUSIC]="LATENCY_ACTION_CREATOR_CHANNEL_MUSIC";W[W.LATENCY_ACTION_CREATOR_CHANNEL_MONETIZATION]="LATENCY_ACTION_CREATOR_CHANNEL_MONETIZATION";W[W.LATENCY_ACTION_CREATOR_CHANNEL_EDITING]="LATENCY_ACTION_CREATOR_CHANNEL_EDITING";W[W.LATENCY_ACTION_CREATOR_CHANNEL_DASHBOARD]="LATENCY_ACTION_CREATOR_CHANNEL_DASHBOARD";
W[W.LATENCY_ACTION_CREATOR_CHANNEL_COPYRIGHT]="LATENCY_ACTION_CREATOR_CHANNEL_COPYRIGHT";W[W.LATENCY_ACTION_CREATOR_CHANNEL_COMMENTS]="LATENCY_ACTION_CREATOR_CHANNEL_COMMENTS";W[W.LATENCY_ACTION_CREATOR_CHANNEL_ANALYTICS]="LATENCY_ACTION_CREATOR_CHANNEL_ANALYTICS";W[W.LATENCY_ACTION_CREATOR_ARTIST_PROFILE]="LATENCY_ACTION_CREATOR_ARTIST_PROFILE";W[W.LATENCY_ACTION_CREATOR_ARTIST_CONCERTS]="LATENCY_ACTION_CREATOR_ARTIST_CONCERTS";W[W.LATENCY_ACTION_CREATOR_ARTIST_ANALYTICS]="LATENCY_ACTION_CREATOR_ARTIST_ANALYTICS";
W[W.LATENCY_ACTION_CREATOR_ANALYTICS_EXPLORE]="LATENCY_ACTION_CREATOR_ANALYTICS_EXPLORE";W[W.LATENCY_ACTION_STORYBOARD_THUMBNAILS]="LATENCY_ACTION_STORYBOARD_THUMBNAILS";W[W.LATENCY_ACTION_SEARCH_THUMBNAILS]="LATENCY_ACTION_SEARCH_THUMBNAILS";W[W.LATENCY_ACTION_ON_DEVICE_MODEL_DOWNLOAD]="LATENCY_ACTION_ON_DEVICE_MODEL_DOWNLOAD";W[W.LATENCY_ACTION_VOICE_ASSISTANT]="LATENCY_ACTION_VOICE_ASSISTANT";W[W.LATENCY_ACTION_SEARCH_UI]="LATENCY_ACTION_SEARCH_UI";W[W.LATENCY_ACTION_SUGGEST]="LATENCY_ACTION_SUGGEST";
W[W.LATENCY_ACTION_AUTO_SEARCH]="LATENCY_ACTION_AUTO_SEARCH";W[W.LATENCY_ACTION_DOWNLOADS]="LATENCY_ACTION_DOWNLOADS";W[W.LATENCY_ACTION_EXPLORE]="LATENCY_ACTION_EXPLORE";W[W.LATENCY_ACTION_VIDEO_LIST]="LATENCY_ACTION_VIDEO_LIST";W[W.LATENCY_ACTION_HOME_RESUME]="LATENCY_ACTION_HOME_RESUME";W[W.LATENCY_ACTION_SUBSCRIPTIONS_LIST]="LATENCY_ACTION_SUBSCRIPTIONS_LIST";W[W.LATENCY_ACTION_THUMBNAIL_LOAD]="LATENCY_ACTION_THUMBNAIL_LOAD";W[W.LATENCY_ACTION_FIRST_THUMBNAIL_LOAD]="LATENCY_ACTION_FIRST_THUMBNAIL_LOAD";
W[W.LATENCY_ACTION_SUBSCRIPTIONS_FEED]="LATENCY_ACTION_SUBSCRIPTIONS_FEED";W[W.LATENCY_ACTION_SUBSCRIPTIONS]="LATENCY_ACTION_SUBSCRIPTIONS";W[W.LATENCY_ACTION_TRENDING]="LATENCY_ACTION_TRENDING";W[W.LATENCY_ACTION_LIBRARY]="LATENCY_ACTION_LIBRARY";W[W.LATENCY_ACTION_VIDEO_THUMBNAIL]="LATENCY_ACTION_VIDEO_THUMBNAIL";W[W.LATENCY_ACTION_SHOW_MORE]="LATENCY_ACTION_SHOW_MORE";W[W.LATENCY_ACTION_VIDEO_PREVIEW]="LATENCY_ACTION_VIDEO_PREVIEW";W[W.LATENCY_ACTION_PREBUFFER_VIDEO]="LATENCY_ACTION_PREBUFFER_VIDEO";
W[W.LATENCY_ACTION_PREFETCH_VIDEO]="LATENCY_ACTION_PREFETCH_VIDEO";W[W.LATENCY_ACTION_DIRECT_PLAYBACK]="LATENCY_ACTION_DIRECT_PLAYBACK";W[W.LATENCY_ACTION_REEL_WATCH]="LATENCY_ACTION_REEL_WATCH";W[W.LATENCY_ACTION_AD_TO_AD]="LATENCY_ACTION_AD_TO_AD";W[W.LATENCY_ACTION_VIDEO_TO_AD]="LATENCY_ACTION_VIDEO_TO_AD";W[W.LATENCY_ACTION_AD_TO_VIDEO]="LATENCY_ACTION_AD_TO_VIDEO";W[W.LATENCY_ACTION_ONBOARDING]="LATENCY_ACTION_ONBOARDING";W[W.LATENCY_ACTION_LOGIN]="LATENCY_ACTION_LOGIN";
W[W.LATENCY_ACTION_BROWSE]="LATENCY_ACTION_BROWSE";W[W.LATENCY_ACTION_CHANNELS]="LATENCY_ACTION_CHANNELS";W[W.LATENCY_ACTION_WATCH]="LATENCY_ACTION_WATCH";W[W.LATENCY_ACTION_RESULTS]="LATENCY_ACTION_RESULTS";W[W.LATENCY_ACTION_HOME]="LATENCY_ACTION_HOME";W[W.LATENCY_ACTION_STARTUP]="LATENCY_ACTION_STARTUP";W[W.LATENCY_ACTION_UNKNOWN]="LATENCY_ACTION_UNKNOWN";var Qr={LATENCY_NETWORK_MOBILE:2,LATENCY_NETWORK_WIFI:1,LATENCY_NETWORK_UNKNOWN:0};Qr[Qr.LATENCY_NETWORK_MOBILE]="LATENCY_NETWORK_MOBILE";
Qr[Qr.LATENCY_NETWORK_WIFI]="LATENCY_NETWORK_WIFI";Qr[Qr.LATENCY_NETWORK_UNKNOWN]="LATENCY_NETWORK_UNKNOWN";var X={CONN_INVALID:31,CONN_CELLULAR_5G_NSA:12,CONN_CELLULAR_5G_SA:11,CONN_WIFI_METERED:10,CONN_CELLULAR_5G:9,CONN_DISCO:8,CONN_CELLULAR_UNKNOWN:7,CONN_CELLULAR_4G:6,CONN_CELLULAR_3G:5,CONN_CELLULAR_2G:4,CONN_WIFI:3,CONN_NONE:2,CONN_UNKNOWN:1,CONN_DEFAULT:0};X[X.CONN_INVALID]="CONN_INVALID";X[X.CONN_CELLULAR_5G_NSA]="CONN_CELLULAR_5G_NSA";X[X.CONN_CELLULAR_5G_SA]="CONN_CELLULAR_5G_SA";
X[X.CONN_WIFI_METERED]="CONN_WIFI_METERED";X[X.CONN_CELLULAR_5G]="CONN_CELLULAR_5G";X[X.CONN_DISCO]="CONN_DISCO";X[X.CONN_CELLULAR_UNKNOWN]="CONN_CELLULAR_UNKNOWN";X[X.CONN_CELLULAR_4G]="CONN_CELLULAR_4G";X[X.CONN_CELLULAR_3G]="CONN_CELLULAR_3G";X[X.CONN_CELLULAR_2G]="CONN_CELLULAR_2G";X[X.CONN_WIFI]="CONN_WIFI";X[X.CONN_NONE]="CONN_NONE";X[X.CONN_UNKNOWN]="CONN_UNKNOWN";X[X.CONN_DEFAULT]="CONN_DEFAULT";
var Y={DETAILED_NETWORK_TYPE_NR_NSA:126,DETAILED_NETWORK_TYPE_NR_SA:125,DETAILED_NETWORK_TYPE_INTERNAL_WIFI_IMPAIRED:124,DETAILED_NETWORK_TYPE_APP_WIFI_HOTSPOT:123,DETAILED_NETWORK_TYPE_DISCONNECTED:122,DETAILED_NETWORK_TYPE_NON_MOBILE_UNKNOWN:121,DETAILED_NETWORK_TYPE_MOBILE_UNKNOWN:120,DETAILED_NETWORK_TYPE_WIMAX:119,DETAILED_NETWORK_TYPE_ETHERNET:118,DETAILED_NETWORK_TYPE_BLUETOOTH:117,DETAILED_NETWORK_TYPE_WIFI:116,DETAILED_NETWORK_TYPE_LTE:115,DETAILED_NETWORK_TYPE_HSPAP:114,DETAILED_NETWORK_TYPE_EHRPD:113,
DETAILED_NETWORK_TYPE_EVDO_B:112,DETAILED_NETWORK_TYPE_UMTS:111,DETAILED_NETWORK_TYPE_IDEN:110,DETAILED_NETWORK_TYPE_HSUPA:109,DETAILED_NETWORK_TYPE_HSPA:108,DETAILED_NETWORK_TYPE_HSDPA:107,DETAILED_NETWORK_TYPE_EVDO_A:106,DETAILED_NETWORK_TYPE_EVDO_0:105,DETAILED_NETWORK_TYPE_CDMA:104,DETAILED_NETWORK_TYPE_1_X_RTT:103,DETAILED_NETWORK_TYPE_GPRS:102,DETAILED_NETWORK_TYPE_EDGE:101,DETAILED_NETWORK_TYPE_UNKNOWN:0};Y[Y.DETAILED_NETWORK_TYPE_NR_NSA]="DETAILED_NETWORK_TYPE_NR_NSA";
Y[Y.DETAILED_NETWORK_TYPE_NR_SA]="DETAILED_NETWORK_TYPE_NR_SA";Y[Y.DETAILED_NETWORK_TYPE_INTERNAL_WIFI_IMPAIRED]="DETAILED_NETWORK_TYPE_INTERNAL_WIFI_IMPAIRED";Y[Y.DETAILED_NETWORK_TYPE_APP_WIFI_HOTSPOT]="DETAILED_NETWORK_TYPE_APP_WIFI_HOTSPOT";Y[Y.DETAILED_NETWORK_TYPE_DISCONNECTED]="DETAILED_NETWORK_TYPE_DISCONNECTED";Y[Y.DETAILED_NETWORK_TYPE_NON_MOBILE_UNKNOWN]="DETAILED_NETWORK_TYPE_NON_MOBILE_UNKNOWN";Y[Y.DETAILED_NETWORK_TYPE_MOBILE_UNKNOWN]="DETAILED_NETWORK_TYPE_MOBILE_UNKNOWN";
Y[Y.DETAILED_NETWORK_TYPE_WIMAX]="DETAILED_NETWORK_TYPE_WIMAX";Y[Y.DETAILED_NETWORK_TYPE_ETHERNET]="DETAILED_NETWORK_TYPE_ETHERNET";Y[Y.DETAILED_NETWORK_TYPE_BLUETOOTH]="DETAILED_NETWORK_TYPE_BLUETOOTH";Y[Y.DETAILED_NETWORK_TYPE_WIFI]="DETAILED_NETWORK_TYPE_WIFI";Y[Y.DETAILED_NETWORK_TYPE_LTE]="DETAILED_NETWORK_TYPE_LTE";Y[Y.DETAILED_NETWORK_TYPE_HSPAP]="DETAILED_NETWORK_TYPE_HSPAP";Y[Y.DETAILED_NETWORK_TYPE_EHRPD]="DETAILED_NETWORK_TYPE_EHRPD";Y[Y.DETAILED_NETWORK_TYPE_EVDO_B]="DETAILED_NETWORK_TYPE_EVDO_B";
Y[Y.DETAILED_NETWORK_TYPE_UMTS]="DETAILED_NETWORK_TYPE_UMTS";Y[Y.DETAILED_NETWORK_TYPE_IDEN]="DETAILED_NETWORK_TYPE_IDEN";Y[Y.DETAILED_NETWORK_TYPE_HSUPA]="DETAILED_NETWORK_TYPE_HSUPA";Y[Y.DETAILED_NETWORK_TYPE_HSPA]="DETAILED_NETWORK_TYPE_HSPA";Y[Y.DETAILED_NETWORK_TYPE_HSDPA]="DETAILED_NETWORK_TYPE_HSDPA";Y[Y.DETAILED_NETWORK_TYPE_EVDO_A]="DETAILED_NETWORK_TYPE_EVDO_A";Y[Y.DETAILED_NETWORK_TYPE_EVDO_0]="DETAILED_NETWORK_TYPE_EVDO_0";Y[Y.DETAILED_NETWORK_TYPE_CDMA]="DETAILED_NETWORK_TYPE_CDMA";
Y[Y.DETAILED_NETWORK_TYPE_1_X_RTT]="DETAILED_NETWORK_TYPE_1_X_RTT";Y[Y.DETAILED_NETWORK_TYPE_GPRS]="DETAILED_NETWORK_TYPE_GPRS";Y[Y.DETAILED_NETWORK_TYPE_EDGE]="DETAILED_NETWORK_TYPE_EDGE";Y[Y.DETAILED_NETWORK_TYPE_UNKNOWN]="DETAILED_NETWORK_TYPE_UNKNOWN";var Rr={LATENCY_PLAYER_RTSP:7,LATENCY_PLAYER_HTML5_INLINE:6,LATENCY_PLAYER_HTML5_FULLSCREEN:5,LATENCY_PLAYER_HTML5:4,LATENCY_PLAYER_FRAMEWORK:3,LATENCY_PLAYER_FLASH:2,LATENCY_PLAYER_EXO:1,LATENCY_PLAYER_UNKNOWN:0};Rr[Rr.LATENCY_PLAYER_RTSP]="LATENCY_PLAYER_RTSP";
Rr[Rr.LATENCY_PLAYER_HTML5_INLINE]="LATENCY_PLAYER_HTML5_INLINE";Rr[Rr.LATENCY_PLAYER_HTML5_FULLSCREEN]="LATENCY_PLAYER_HTML5_FULLSCREEN";Rr[Rr.LATENCY_PLAYER_HTML5]="LATENCY_PLAYER_HTML5";Rr[Rr.LATENCY_PLAYER_FRAMEWORK]="LATENCY_PLAYER_FRAMEWORK";Rr[Rr.LATENCY_PLAYER_FLASH]="LATENCY_PLAYER_FLASH";Rr[Rr.LATENCY_PLAYER_EXO]="LATENCY_PLAYER_EXO";Rr[Rr.LATENCY_PLAYER_UNKNOWN]="LATENCY_PLAYER_UNKNOWN";
var Sr={LATENCY_AD_BREAK_TYPE_POSTROLL:3,LATENCY_AD_BREAK_TYPE_MIDROLL:2,LATENCY_AD_BREAK_TYPE_PREROLL:1,LATENCY_AD_BREAK_TYPE_UNKNOWN:0};Sr[Sr.LATENCY_AD_BREAK_TYPE_POSTROLL]="LATENCY_AD_BREAK_TYPE_POSTROLL";Sr[Sr.LATENCY_AD_BREAK_TYPE_MIDROLL]="LATENCY_AD_BREAK_TYPE_MIDROLL";Sr[Sr.LATENCY_AD_BREAK_TYPE_PREROLL]="LATENCY_AD_BREAK_TYPE_PREROLL";Sr[Sr.LATENCY_AD_BREAK_TYPE_UNKNOWN]="LATENCY_AD_BREAK_TYPE_UNKNOWN";var Tr={LATENCY_ACTION_ERROR_STARTUP_TIMEOUT:1,LATENCY_ACTION_ERROR_UNSPECIFIED:0};
Tr[Tr.LATENCY_ACTION_ERROR_STARTUP_TIMEOUT]="LATENCY_ACTION_ERROR_STARTUP_TIMEOUT";Tr[Tr.LATENCY_ACTION_ERROR_UNSPECIFIED]="LATENCY_ACTION_ERROR_UNSPECIFIED";var Ur={LIVE_STREAM_MODE_WINDOW:5,LIVE_STREAM_MODE_POST:4,LIVE_STREAM_MODE_LP:3,LIVE_STREAM_MODE_LIVE:2,LIVE_STREAM_MODE_DVR:1,LIVE_STREAM_MODE_UNKNOWN:0};Ur[Ur.LIVE_STREAM_MODE_WINDOW]="LIVE_STREAM_MODE_WINDOW";Ur[Ur.LIVE_STREAM_MODE_POST]="LIVE_STREAM_MODE_POST";Ur[Ur.LIVE_STREAM_MODE_LP]="LIVE_STREAM_MODE_LP";
Ur[Ur.LIVE_STREAM_MODE_LIVE]="LIVE_STREAM_MODE_LIVE";Ur[Ur.LIVE_STREAM_MODE_DVR]="LIVE_STREAM_MODE_DVR";Ur[Ur.LIVE_STREAM_MODE_UNKNOWN]="LIVE_STREAM_MODE_UNKNOWN";var Vr={VIDEO_STREAM_TYPE_VOD:3,VIDEO_STREAM_TYPE_DVR:2,VIDEO_STREAM_TYPE_LIVE:1,VIDEO_STREAM_TYPE_UNSPECIFIED:0};Vr[Vr.VIDEO_STREAM_TYPE_VOD]="VIDEO_STREAM_TYPE_VOD";Vr[Vr.VIDEO_STREAM_TYPE_DVR]="VIDEO_STREAM_TYPE_DVR";Vr[Vr.VIDEO_STREAM_TYPE_LIVE]="VIDEO_STREAM_TYPE_LIVE";Vr[Vr.VIDEO_STREAM_TYPE_UNSPECIFIED]="VIDEO_STREAM_TYPE_UNSPECIFIED";
var Wr={YT_IDB_TRANSACTION_TYPE_READ:2,YT_IDB_TRANSACTION_TYPE_WRITE:1,YT_IDB_TRANSACTION_TYPE_UNKNOWN:0};Wr[Wr.YT_IDB_TRANSACTION_TYPE_READ]="YT_IDB_TRANSACTION_TYPE_READ";Wr[Wr.YT_IDB_TRANSACTION_TYPE_WRITE]="YT_IDB_TRANSACTION_TYPE_WRITE";Wr[Wr.YT_IDB_TRANSACTION_TYPE_UNKNOWN]="YT_IDB_TRANSACTION_TYPE_UNKNOWN";var Xr={PLAYER_ROTATION_TYPE_PORTRAIT_TO_FULLSCREEN:2,PLAYER_ROTATION_TYPE_FULLSCREEN_TO_PORTRAIT:1,PLAYER_ROTATION_TYPE_UNKNOWN:0};Xr[Xr.PLAYER_ROTATION_TYPE_PORTRAIT_TO_FULLSCREEN]="PLAYER_ROTATION_TYPE_PORTRAIT_TO_FULLSCREEN";
Xr[Xr.PLAYER_ROTATION_TYPE_FULLSCREEN_TO_PORTRAIT]="PLAYER_ROTATION_TYPE_FULLSCREEN_TO_PORTRAIT";Xr[Xr.PLAYER_ROTATION_TYPE_UNKNOWN]="PLAYER_ROTATION_TYPE_UNKNOWN";var Yr="actionVisualElement spinnerInfo resourceInfo playerInfo commentInfo mdxInfo watchInfo thumbnailLoadInfo creatorInfo unpluggedInfo reelInfo subscriptionsFeedInfo requestIds mediaBrowserActionInfo musicLoadActionInfo shoppingInfo prefetchInfo accelerationSession webInfo tvInfo kabukiInfo mwebInfo musicInfo".split(" ");var Zr=x.ytLoggingLatencyUsageStats_||{};y("ytLoggingLatencyUsageStats_",Zr,void 0);function $r(){this.h=0}
function as(){$r.h||($r.h=new $r);return $r.h}
$r.prototype.tick=function(a,b,c,d){bs(this,"tick_"+a+"_"+b)||(c={timestamp:c,cttAuthInfo:d},M("web_csi_via_jspb")?(d=new Ah,D(d,1,a),D(d,2,b),a=new Dh,nd(a,5,Eh,d),Dp(a,c)):ok("latencyActionTicked",{tickName:a,clientActionNonce:b},c))};
$r.prototype.info=function(a,b,c){var d=Object.keys(a).join("");bs(this,"info_"+d+"_"+b)||(a=Object.assign({},a),a.clientActionNonce=b,ok("latencyActionInfo",a,{cttAuthInfo:c}))};
$r.prototype.jspbInfo=function(a,b,c){for(var d="",e=0;e<a.toJSON().length;e++)void 0!==a.toJSON()[e]&&(d=0===e?d.concat(""+e):d.concat("_"+e));bs(this,"info_"+d+"_"+b)||(D(a,2,b),b={cttAuthInfo:c},c=new Dh,nd(c,7,Eh,a),Dp(c,b))};
$r.prototype.span=function(a,b,c){var d=Object.keys(a).join("");bs(this,"span_"+d+"_"+b)||(a.clientActionNonce=b,ok("latencyActionSpan",a,{cttAuthInfo:c}))};
function bs(a,b){Zr[b]=Zr[b]||{count:0};var c=Zr[b];c.count++;c.time=P();a.h||(a.h=Yj(function(){var d=P(),e;for(e in Zr)Zr[e]&&6E4<d-Zr[e].time&&delete Zr[e];a&&(a.h=0)},5E3));
return 5<c.count?(6===c.count&&1>1E5*Math.random()&&(c=new Q("CSI data exceeded logging limit with key",b.split("_")),0<=b.indexOf("plev")||Sp(c)),!0):!1}
;function cs(){var a=["ol"];Hr("").info.actionType="embed";a&&Zh("TIMING_AFT_KEYS",a);Zh("TIMING_ACTION","embed");ds();a=zr();var b=Br();if("cold"===a.yt_lt||"cold"===b.loadType){var c=ur(),d=Ar();d=d.gelTicks?d.gelTicks:d.gelTicks={};for(var e in c)e in d||Z(e,c[e]);e={};c=!1;d=p(Object.keys(a));for(var f=d.next();!f.done;f=d.next())f=f.value,(f=Pr(f,a[f]))&&!Er(Br(void 0),f)&&(ir(b,f),ir(e,f),c=!0);c&&es(e)}Za("ytglobal.timingready_",!0);a=$h("TIMING_ACTION");z("ytglobal.timingready_")&&a&&"_start"in
ur(void 0)&&tr()&&Dr()}
function fs(a,b,c,d){null!==b&&(zr(c)[a]=b,(a=Pr(a,b,c))&&es(a,c,d))}
function es(a,b,c){if(!M("web_csi_via_jspb")||(void 0===c?0:c))c=Hr(b||""),ir(c.info,a),ir(Br(b),a),c=Cr(b),b=yr(b).cttAuthInfo,as().info(a,c,b);else{c=new wh;var d=Object.keys(a);a=Object.values(a);for(var e=0;e<d.length;e++){var f=d[e];try{switch(f){case "actionType":D(c,1,W[a[e]]);break;case "clientActionNonce":D(c,2,a[e]);break;case "clientScreenNonce":D(c,4,a[e]);break;case "loadType":D(c,3,a[e]);break;case "isFirstInstall":D(c,55,a[e]);break;case "networkType":D(c,5,Qr[a[e]]);break;case "connectionType":D(c,
26,X[a[e]]);break;case "detailedConnectionType":D(c,27,Y[a[e]]);break;case "isVisible":D(c,6,a[e]);break;case "playerType":D(c,7,Rr[a[e]]);break;case "clientPlaybackNonce":D(c,8,a[e]);break;case "adClientPlaybackNonce":D(c,28,a[e]);break;case "previousCpn":D(c,77,a[e]);break;case "targetCpn":D(c,76,a[e]);break;case "isMonetized":D(c,9,a[e]);break;case "isPrerollAllowed":D(c,16,a[e]);break;case "isPrerollShown":D(c,17,a[e]);break;case "adType":D(c,12,a[e]);break;case "adTypesAllowed":D(c,36,a[e]);
break;case "adNetworks":D(c,37,a[e]);break;case "previousAction":D(c,13,W[a[e]]);break;case "isRedSubscriber":D(c,14,a[e]);break;case "serverTimeMs":D(c,15,a[e]);break;case "videoId":c.setVideoId(a[e]);break;case "adVideoId":D(c,20,a[e]);break;case "targetVideoId":D(c,78,a[e]);break;case "adBreakType":D(c,21,Sr[a[e]]);break;case "isNavigation":D(c,25,a[e]);break;case "viewportHeight":D(c,29,a[e]);break;case "viewportWidth":D(c,30,a[e]);break;case "screenHeight":D(c,84,a[e]);break;case "screenWidth":D(c,
85,a[e]);break;case "browseId":D(c,31,a[e]);break;case "isCacheHit":D(c,32,a[e]);break;case "httpProtocol":D(c,33,a[e]);break;case "transportProtocol":D(c,34,a[e]);break;case "searchQuery":D(c,41,a[e]);break;case "isContinuation":D(c,42,a[e]);break;case "availableProcessors":D(c,43,a[e]);break;case "sdk":D(c,44,a[e]);break;case "isLocalStream":D(c,45,a[e]);break;case "navigationRequestedSameUrl":D(c,64,a[e]);break;case "shellStartupDurationMs":D(c,70,a[e]);break;case "appInstallDataAgeMs":D(c,73,
a[e]);break;case "latencyActionError":D(c,71,Tr[a[e]]);break;case "actionStep":D(c,79,a[e]);break;case "jsHeapSizeLimit":D(c,80,a[e]);break;case "totalJsHeapSize":D(c,81,a[e]);break;case "usedJsHeapSize":D(c,82,a[e]);break;case "sourceVideoDurationMs":D(c,90,a[e]);break;case "adPrebufferedTimeSecs":D(c,39,a[e]);break;case "isLivestream":D(c,47,a[e]);break;case "liveStreamMode":D(c,91,Ur[a[e]]);break;case "adCpn2":D(c,48,a[e]);break;case "adDaiDriftMillis":D(c,49,a[e]);break;case "videoStreamType":D(c,
53,Vr[a[e]]);break;case "playbackRequiresTap":D(c,56,a[e]);break;case "performanceNavigationTiming":D(c,67,a[e]);break;case "transactionType":D(c,74,Wr[a[e]]);break;case "playerRotationType":D(c,101,Xr[a[e]]);break;case "allowedPreroll":D(c,10,a[e]);break;case "shownPreroll":D(c,11,a[e]);break;case "getHomeRequestId":D(c,57,a[e]);break;case "getSearchRequestId":D(c,60,a[e]);break;case "getPlayerRequestId":D(c,61,a[e]);break;case "getWatchNextRequestId":D(c,62,a[e]);break;case "getBrowseRequestId":D(c,
63,a[e]);break;case "getLibraryRequestId":D(c,66,a[e]);break;default:Yr.includes(f)&&ii(new Q("Codegen laipb translator asked to translate message field",""+f))}}catch(g){ii(Error("Codegen laipb translator failed to set "+f))}}gs(c,b)}}
function gs(a,b){var c=Ar(b);c.jspbInfos||(c.jspbInfos=[]);c.jspbInfos.push(a);Hr(b||"").jspbInfo.push(a);c=Cr(b);b=yr(b).cttAuthInfo;as().jspbInfo(a,c,b)}
function Z(a,b,c){if(!b&&"_"!==a[0]){var d=a;T.mark&&(0==d.lastIndexOf("mark_",0)||(d="mark_"+d),c&&(d+=" ("+c+")"),T.mark(d))}Hr(c||"").tick[a]=b||P();d=Ar(c);d.gelTicks&&(d.gelTicks[a]=!0);d=ur(c);var e=b||P();d[a]=e;e=Cr(c);var f=yr(c).cttAuthInfo;if("_start"===a){var g=as();bs(g,"baseline_"+e)||(b={timestamp:b,cttAuthInfo:f},M("web_csi_via_jspb")?(f=new uh,D(f,1,e),e=new Dh,nd(e,6,Eh,f),Dp(e,b)):ok("latencyActionBaselined",{clientActionNonce:e},b))}else as().tick(a,e,b,f);Dr(c);return d[a]}
function hs(){var a=Cr(void 0);requestAnimationFrame(function(){setTimeout(function(){a===Cr(void 0)&&Z("ol",void 0,void 0)},0)})}
function is(){var a=document;if("visibilityState"in a)a=a.visibilityState;else{var b=wn+"VisibilityState";a=b in a?a[b]:void 0}switch(a){case "hidden":return 0;case "visible":return 1;case "prerender":return 2;case "unloaded":return 3;default:return-1}}
function ds(){function a(f){var g=vr(),h=xr();h&&(Z("srt",g.responseStart),1!==f.prerender&&Z("_start",h,void 0));f=Fr();0<f&&Z("fpt",f);f=vr();f.isPerformanceNavigationTiming&&es({performanceNavigationTiming:!0},void 0);Z("nreqs",f.requestStart,void 0);Z("nress",f.responseStart,void 0);Z("nrese",f.responseEnd,void 0);0<f.redirectEnd-f.redirectStart&&(Z("nrs",f.redirectStart,void 0),Z("nre",f.redirectEnd,void 0));0<f.domainLookupEnd-f.domainLookupStart&&(Z("ndnss",f.domainLookupStart,void 0),Z("ndnse",
f.domainLookupEnd,void 0));0<f.connectEnd-f.connectStart&&(Z("ntcps",f.connectStart,void 0),Z("ntcpe",f.connectEnd,void 0));f.secureConnectionStart>=xr()&&0<f.connectEnd-f.secureConnectionStart&&(Z("nstcps",f.secureConnectionStart,void 0),Z("ntcpe",f.connectEnd,void 0));T&&"getEntriesByType"in T&&js()}
var b=A("TIMING_INFO",{});if(M("web_csi_via_jspb")){b=ks(b);gs(b);b=new wh;D(b,25,!0);D(b,1,W[Or(A("TIMING_ACTION",void 0))]);var c=A("PREVIOUS_ACTION",void 0);c&&D(b,13,W[Or(c)]);(c=A("CLIENT_PROTOCOL"))&&D(b,33,c);(c=A("CLIENT_TRANSPORT"))&&D(b,34,c);(c=jq())&&"UNDEFINED_CSN"!==c&&D(b,4,c);c=is();1!==c&&-1!==c||D(b,6,!0);c=zr();D(b,3,"cold");a(c);c=ls();if(0<c.length){c=p(c);for(var d=c.next();!d.done;d=c.next()){d=d.value;var e=new vh;D(e,1,d);od(b,83,vh,e)}}gs(b)}else{for(c in b)b.hasOwnProperty(c)&&
fs(c,b[c]);b={isNavigation:!0,actionType:Or(A("TIMING_ACTION",void 0))};if(c=A("PREVIOUS_ACTION",void 0))b.previousAction=Or(c);if(c=A("CLIENT_PROTOCOL"))b.httpProtocol=c;if(c=A("CLIENT_TRANSPORT"))b.transportProtocol=c;(c=jq())&&"UNDEFINED_CSN"!==c&&(b.clientScreenNonce=c);c=is();if(1===c||-1===c)b.isVisible=!0;c=zr();b.loadType="cold";a(c);c=ls();if(0<c.length)for(b.resourceInfo=[],c=p(c),d=c.next();!d.done;d=c.next())b.resourceInfo.push({resourceCache:d.value});es(b)}}
function ks(a){var b=new wh;a=p(Object.entries(a));for(var c=a.next();!c.done;c=a.next()){var d=p(c.value);c=d.next().value;d=d.next().value;switch(c){case "GetBrowse_rid":var e=new zh;D(e,1,c);D(e,2,String(d));yh(b,e);break;case "GetGuide_rid":e=new zh;D(e,1,c);D(e,2,String(d));yh(b,e);break;case "GetHome_rid":e=new zh;D(e,1,c);D(e,2,String(d));yh(b,e);break;case "GetPlayer_rid":e=new zh;D(e,1,c);D(e,2,String(d));yh(b,e);break;case "GetSearch_rid":e=new zh;D(e,1,c);D(e,2,String(d));yh(b,e);break;
case "GetSettings_rid":e=new zh;D(e,1,c);D(e,2,String(d));yh(b,e);break;case "GetTrending_rid":e=new zh;D(e,1,c);D(e,2,String(d));yh(b,e);break;case "GetWatchNext_rid":e=new zh;D(e,1,c);D(e,2,String(d));yh(b,e);break;case "yt_red":D(b,14,!!d);break;case "yt_ad":D(b,9,!!d)}}return b}
function ms(a,b){a=document.querySelector(a);if(!a)return!1;var c="",d=a.nodeName;"SCRIPT"===d?(c=a.src,c||(c=a.getAttribute("data-timing-href"))&&(c=window.location.protocol+c)):"LINK"===d&&(c=a.href);Yb()&&a.setAttribute("nonce",Yb());return c?(a=T.getEntriesByName(c))&&a[0]&&(a=a[0],c=xr(),Z("rsf_"+b,c+Math.round(a.fetchStart)),Z("rse_"+b,c+Math.round(a.responseEnd)),void 0!==a.transferSize&&0===a.transferSize)?!0:!1:!1}
function ls(){var a=[];if(document.querySelector&&T&&T.getEntriesByName)for(var b in sr)if(sr.hasOwnProperty(b)){var c=sr[b];ms(b,c)&&a.push(c)}return a}
function js(){var a=window.location.protocol,b=T.getEntriesByType("resource");b=gb(b,function(c){return 0===c.name.indexOf(a+"//fonts.gstatic.com/s/")});
(b=ib(b,function(c,d){return d.duration>c.duration?d:c},{duration:0}))&&0<b.startTime&&0<b.responseEnd&&(Z("wffs",wr(b.startTime)),Z("wffe",wr(b.responseEnd)))}
var ns=window;ns.ytcsi&&(ns.ytcsi.info=fs,ns.ytcsi.tick=Z);var os=["consistency","mss","client_location","entities","store"];function ps(a,b,c,d){this.o=a;this.J=b;this.l=c;this.j=d;this.i=void 0;this.h=new Map;a.za||(a.za={});a.za=Object.assign({},hr,a.za)}
function qs(a,b,c,d){if(void 0!==ps.h){if(d=ps.h,a=[a!==d.o,b!==d.J,c!==d.l,!1,!1,void 0!==d.i],a.some(function(e){return e}))throw new Q("InnerTubeTransportService is already initialized",a);
}else ps.h=new ps(a,b,c,d)}
function rs(a){var b={signalServiceEndpoint:{signal:"GET_DATASYNC_IDS"}};var c=void 0===c?Qq:c;var d=Zq(b,a.o);if(!d)return eg(new Q("Error: No request builder found for command.",b));var e=d.o(b,void 0,c);return e?new $f(function(f){var g,h,k;return w(function(m){if(1==m.h){h="cors"===(null==(g=e.ra)?void 0:g.mode)?"cors":void 0;if(a.l.zc){var q=e.config,u;q=null==q?void 0:null==(u=q.Ga)?void 0:u.sessionIndex;u=Hq({sessionIndex:q});k=Object.assign({},ss(h),u);return m.s(2)}return t(m,ts(e.config,
h),3)}2!=m.h&&(k=m.i);f(us(a,e,k));m.h=0})}):eg(new Q("Error: Failed to build request for command.",b))}
function us(a,b,c){var d,e,f,g,h,k,m,q,u,C,v,E,F,H,R,O,S,ia;return w(function(N){switch(N.h){case 1:N.s(2);break;case 3:if((d=N.i)&&!d.isExpired())return N.return(Promise.resolve(d.h()));case 2:if((f=null==(e=b.config)?void 0:e.No)&&a.h.has(f)&&M("web_memoize_inflight_requests"))return N.return(a.h.get(f));if(null==(g=b)?0:null==(h=g.ba)?0:h.context)for(k=p([]),m=k.next();!m.done;m=k.next())q=m.value,q.Lo(b.ba.context);if(null==(u=a.i)?0:u.l(b.input,b.ba))return N.return(a.i.j(b.input,b.ba));C=JSON.stringify(b.ba);
b.ra=Object.assign({},b.ra,{headers:c});v=Object.assign({},b.ra);"POST"===b.ra.method&&(v=Object.assign({},v,{body:C}));(null==(E=b.config)?0:E.kc)&&Z(b.config.kc);F=a.J.fetch(b.input,v,b.config);f&&a.h.set(f,F);return t(N,F,4);case 4:H=N.i;f&&a.h.has(f)&&a.h.delete(f);(null==(R=b.config)?0:R.lc)&&Z(b.config.lc);if(H||null==(O=a.i)||!O.h(b.input,b.ba)){N.s(5);break}return t(N,a.i.i(b.input,b.ba),6);case 6:H=N.i;case 5:if(H&&a.j)for(S=p(os),m=S.next();!m.done;m=S.next())ia=m.value,a.j[ia]&&a.j[ia].handleResponse(H,
b);return N.return(H)}})}
function ts(a,b){var c,d,e,f;return w(function(g){if(1==g.h){e=null==(c=a)?void 0:null==(d=c.Ga)?void 0:d.sessionIndex;var h=Hq({sessionIndex:e});if(!(h instanceof $f)){var k=new $f(Ma);ag(k,2,h);h=k}return t(g,h,2)}f=g.i;return g.return(Promise.resolve(Object.assign({},ss(b),f)))})}
function ss(a){var b={"Content-Type":"application/json"};M("enable_web_eom_visitor_data")&&A("EOM_VISITOR_DATA")?b["X-Goog-EOM-Visitor-Id"]=A("EOM_VISITOR_DATA"):A("VISITOR_DATA")&&(b["X-Goog-Visitor-Id"]=A("VISITOR_DATA"));"cors"!==a&&((a=A("INNERTUBE_CONTEXT_CLIENT_NAME"))&&(b["X-Youtube-Client-Name"]=a),(a=A("INNERTUBE_CONTEXT_CLIENT_VERSION"))&&(b["X-Youtube-Client-Version"]=a),(a=A("CHROME_CONNECTED_HEADER"))&&(b["X-Youtube-Chrome-Connected"]=a),M("forward_domain_admin_state_on_embeds")&&(a=
A("DOMAIN_ADMIN_STATE"))&&(b["X-Youtube-Domain-Admin-State"]=a));return b}
;var vs=["share/get_web_player_share_panel"],ws=["feedback"],xs=["notification/modify_channel_preference"],ys=["browse/edit_playlist"],zs=["subscription/subscribe"],As=["subscription/unsubscribe"];function Bs(){}
r(Bs,er);Bs.prototype.j=function(){return zs};
Bs.prototype.h=function(a){return a.subscribeEndpoint};
Bs.prototype.i=function(a,b,c){c=void 0===c?{}:c;b.channelIds&&(a.channelIds=b.channelIds);b.siloName&&(a.siloName=b.siloName);b.params&&(a.params=b.params);c.botguardResponse&&(a.botguardResponse=c.botguardResponse);c.feature&&(a.clientFeature=c.feature)};
da.Object.defineProperties(Bs.prototype,{l:{configurable:!0,enumerable:!0,get:function(){return!0}}});function Cs(){}
r(Cs,er);Cs.prototype.j=function(){return As};
Cs.prototype.h=function(a){return a.unsubscribeEndpoint};
Cs.prototype.i=function(a,b){b.channelIds&&(a.channelIds=b.channelIds);b.siloName&&(a.siloName=b.siloName);b.params&&(a.params=b.params)};
da.Object.defineProperties(Cs.prototype,{l:{configurable:!0,enumerable:!0,get:function(){return!0}}});function Ds(){}
r(Ds,er);Ds.prototype.j=function(){return ws};
Ds.prototype.h=function(a){return a.feedbackEndpoint};
Ds.prototype.i=function(a,b,c){a.feedbackTokens=[];b.feedbackToken&&a.feedbackTokens.push(b.feedbackToken);if(b=b.cpn||c.cpn)a.feedbackContext={cpn:b};a.isFeedbackTokenUnencrypted=!!c.is_feedback_token_unencrypted;a.shouldMerge=!1;c.extra_feedback_tokens&&(a.shouldMerge=!0,a.feedbackTokens=a.feedbackTokens.concat(c.extra_feedback_tokens))};
da.Object.defineProperties(Ds.prototype,{l:{configurable:!0,enumerable:!0,get:function(){return!0}}});function Es(){}
r(Es,er);Es.prototype.j=function(){return xs};
Es.prototype.h=function(a){return a.modifyChannelNotificationPreferenceEndpoint};
Es.prototype.i=function(a,b){b.params&&(a.params=b.params);b.secondaryParams&&(a.secondaryParams=b.secondaryParams)};function Fs(){}
r(Fs,er);Fs.prototype.j=function(){return ys};
Fs.prototype.h=function(a){return a.playlistEditEndpoint};
Fs.prototype.i=function(a,b){b.actions&&(a.actions=b.actions);b.params&&(a.params=b.params);b.playlistId&&(a.playlistId=b.playlistId)};function Gs(){}
r(Gs,er);Gs.prototype.j=function(){return vs};
Gs.prototype.h=function(a){return a.webPlayerShareEntityServiceEndpoint};
Gs.prototype.i=function(a,b,c){c=void 0===c?{}:c;b.serializedShareEntity&&(a.serializedSharedEntity=b.serializedShareEntity);c.includeListId&&(a.includeListId=!0)};var Lq=new Jq("NETWORK_SLI_TOKEN");function Hs(a){this.h=a}
Hs.prototype.fetch=function(a,b){var c=this,d,e;return w(function(f){c.h&&(d=bc(cc(5,lc(a)))||"/UNKNOWN_PATH",c.h.start(d));e=new window.Request(a,b);return M("web_fetch_promise_cleanup_killswitch")?f.return(Promise.resolve(fetch(e).then(function(g){return c.handleResponse(g)}).catch(function(g){Sp(g)}))):f.return(fetch(e).then(function(g){return c.handleResponse(g)}).catch(function(g){Sp(g)}))})};
Hs.prototype.handleResponse=function(a){var b=a.text().then(function(c){return JSON.parse(c.replace(")]}'",""))});
a.redirected||a.ok?this.h&&this.h.success():(this.h&&this.h.failure(),b=b.then(function(c){Sp(new Q("Error: API fetch failed",a.status,a.url,c));return Object.assign({},c,{errorMetadata:{status:a.status}})}));
return b};
Hs[Iq]=[new Kq];var Is=new Jq("NETWORK_MANAGER_TOKEN");var Js;function Ks(a){Ul.call(this,1,arguments);this.csn=a}
r(Ks,Ul);var cm=new Vl("screen-created",Ks),Ls=[],Ns=Ms,Os=0;function Ps(a,b,c,d,e,f,g){function h(){Sp(new Q("newScreen() parent element does not have a VE - rootVe",b))}
var k=Ns();f=new cq({veType:b,youtubeData:f,jspbYoutubeData:void 0});e={cttAuthInfo:e,W:k};if(M("il_via_jspb")){var m=new kh;m.X(k);lh(m,f.getAsJspb());c&&c.visualElement?(f=new mh,c.clientScreenNonce&&D(f,2,c.clientScreenNonce),nh(f,c.visualElement.getAsJspb()),g&&D(f,4,jh[g]),I(m,5,f)):c&&h();d&&D(m,3,d);Ip(m,e,a)}else f={csn:k,pageVe:f.getAsJson()},c&&c.visualElement?(f.implicitGesture={parentCsn:c.clientScreenNonce,gesturedVe:c.visualElement.getAsJson()},g&&(f.implicitGesture.gestureType=g)):
c&&h(),d&&(f.cloneCsn=d),a?np("screenCreated",f,a,e):ok("screenCreated",f,e);$l(cm,new Ks(k));return k}
function Qs(a,b,c,d){var e=d.filter(function(k){k.csn!==b?(k.csn=b,k=!0):k=!1;return k}),f={cttAuthInfo:lq(b),
W:b};d=p(d);for(var g=d.next();!g.done;g=d.next())g=g.value.getAsJson(),(pb(g)||!g.trackingParams&&!g.veType)&&Sp(Error("Child VE logged with no data"));if(M("il_via_jspb")){var h=new oh;h.X(b);qh(h,c.getAsJspb());hb(e,function(k){k=k.getAsJspb();od(h,3,hh,k)});
"UNDEFINED_CSN"==b?Rs("visualElementAttached",h,f):Jp(h,f,a)}else c={csn:b,parentVe:c.getAsJson(),childVes:hb(e,function(k){return k.getAsJson()})},"UNDEFINED_CSN"==b?Rs("visualElementAttached",c,f):a?np("visualElementAttached",c,a,f):ok("visualElementAttached",c,f)}
function Ms(){for(var a=Math.random()+"",b=[],c=0,d=0;d<a.length;d++){var e=a.charCodeAt(d);255<e&&(b[c++]=e&255,e>>=8);b[c++]=e}return Lc(b,3)}
function Rs(a,b,c){Ls.push({payloadName:a,payload:b,options:c});Os||(Os=dm())}
function em(a){if(Ls){for(var b=p(Ls),c=b.next();!c.done;c=b.next())if(c=c.value,c.payload)if(M("il_via_jspb"))switch(c.payload.X(a.csn),c.payloadName){case "screenCreated":Ip(c.payload,c.options);break;case "visualElementAttached":Jp(c.payload,c.options);break;case "visualElementShown":Ep(c.payload,c.options);break;case "visualElementHidden":Fp(c.payload,c.options);break;case "visualElementGestured":Gp(c.payload,c.options);break;case "visualElementStateChanged":Hp(c.payload,c.options);break;default:Sp(new Q("flushQueue unable to map payloadName to JSPB setter"))}else c.payload.csn=
a.csn,np(c.payloadName,c.payload,null,c.options);Ls.length=0}Os=0}
;function Ss(){this.i=new Set;this.h=new Set;this.j=new Map}
Ss.prototype.clear=function(){this.i.clear();this.h.clear();this.j.clear()};
Oa(Ss);function Ts(){this.o=[];this.C=[];this.h=[];this.l=[];this.m=[];this.i=new Set;this.u=new Map}
function Us(a,b,c){c=void 0===c?0:c;b.then(function(d){a.i.has(c)&&a.j&&a.j();var e=jq(c),f=hq(c);if(e&&f){var g;(null==d?0:null==(g=d.response)?0:g.trackingParams)&&Qs(a.client,e,f,[dq(d.response.trackingParams)]);var h;(null==d?0:null==(h=d.playerResponse)?0:h.trackingParams)&&Qs(a.client,e,f,[dq(d.playerResponse.trackingParams)])}})}
function Vs(a,b,c,d){d=void 0===d?0:d;if(a.i.has(d))a.o.push([b,c]);else{var e=jq(d);c=c||hq(d);e&&c&&Qs(a.client,e,c,[b])}}
Ts.prototype.clickCommand=function(a,b,c){var d=a.clickTrackingParams;c=void 0===c?0:c;if(d)if(c=jq(void 0===c?0:c)){a=this.client;var e=dq(d);var f="INTERACTION_LOGGING_GESTURE_TYPE_GENERIC_CLICK";d={cttAuthInfo:lq(c),W:c};if(M("il_via_jspb")){var g=new rh;g.X(c);e=e.getAsJspb();I(g,2,e);D(g,4,jh[f]);b&&I(g,3,void 0);"UNDEFINED_CSN"==c?Rs("visualElementGestured",g,d):Gp(g,d,a)}else f={csn:c,ve:e.getAsJson(),gestureType:f},b&&(f.clientData=b),"UNDEFINED_CSN"==c?Rs("visualElementGestured",f,d):a?np("visualElementGestured",
f,a,d):ok("visualElementGestured",f,d);b=!0}else b=!1;else b=!1;return b};
function Ws(a,b,c){c=void 0===c?{}:c;a.i.add(c.layer||0);a.j=function(){Xs(a,b,c);var f=hq(c.layer);if(f){for(var g=p(a.o),h=g.next();!h.done;h=g.next())h=h.value,Vs(a,h[0],h[1]||f,c.layer);f=p(a.C);for(g=f.next();!g.done;g=f.next()){var k=g.value;g=void 0;g=void 0===g?0:g;h=jq(g);var m=k[0]||hq(g);if(h&&m){g=a.client;var q=k[1];k={cttAuthInfo:lq(h),W:h};M("il_via_jspb")?(q=new th,q.X(h),m=m.getAsJspb(),I(q,2,m),"UNDEFINED_CSN"==h?Rs("visualElementStateChanged",q,k):Hp(q,k,g)):(m={csn:h,ve:m.getAsJson(),
clientData:q},"UNDEFINED_CSN"==h?Rs("visualElementStateChanged",m,k):g?np("visualElementStateChanged",m,g,k):ok("visualElementStateChanged",m,k))}}}};
jq(c.layer)||a.j();if(c.rb)for(var d=p(c.rb),e=d.next();!e.done;e=d.next())Us(a,e.value,c.layer);else Rp(Error("Delayed screen needs a data promise."))}
function Xs(a,b,c){c=void 0===c?{}:c;c.layer||(c.layer=0);var d=void 0!==c.ec?c.ec:c.layer;var e=jq(d);d=hq(d);var f;d&&(void 0!==c.parentCsn?f={clientScreenNonce:c.parentCsn,visualElement:d}:e&&"UNDEFINED_CSN"!==e&&(f={clientScreenNonce:e,visualElement:d}));var g,h=A("EVENT_ID");"UNDEFINED_CSN"===e&&h&&(g={servletData:{serializedServletEventId:h}});try{var k=Ps(a.client,b,f,c.qb,c.cttAuthInfo,g,c.Io)}catch(m){Up(m,{Oo:b,rootVe:d,parentVisualElement:void 0,Fo:e,Ko:f,qb:c.qb});Rp(m);return}mq(k,b,
c.layer,c.cttAuthInfo);if(b=e&&"UNDEFINED_CSN"!==e&&d){a:{b=p(Object.values(bq));for(f=b.next();!f.done;f=b.next())if(jq(f.value)==e){b=!0;break a}b=!1}b=!b}b&&(b=a.client,g=!0,h=(g=void 0===g?!1:g)?16:8,f={cttAuthInfo:lq(e),W:e,tb:g},M("il_via_jspb")?(h=new sh,h.X(e),d=d.getAsJspb(),I(h,2,d),D(h,4,g?16:8),"UNDEFINED_CSN"==e?Rs("visualElementHidden",h,f):Fp(h,f,b)):(d={csn:e,ve:d.getAsJson(),eventType:h},"UNDEFINED_CSN"==e?Rs("visualElementHidden",d,f):b?np("visualElementHidden",d,b,f):ok("visualElementHidden",
d,f)));a.h[a.h.length-1]&&!a.h[a.h.length-1].csn&&(a.h[a.h.length-1].csn=k||"");es({clientScreenNonce:k});Ss.getInstance().clear();d=hq(c.layer);e&&"UNDEFINED_CSN"!==e&&d&&(M("web_mark_root_visible")||M("music_web_mark_root_visible"))&&(e=k,k={cttAuthInfo:lq(e),W:e},M("il_via_jspb")?(b=new sh,b.X(e),f=d.getAsJspb(),I(b,2,f),D(b,4,1),"UNDEFINED_CSN"==e?Rs("visualElementShown",b,k):Ep(b,k,void 0)):(b={csn:e,ve:d.getAsJson(),eventType:1},"UNDEFINED_CSN"==e?Rs("visualElementShown",b,k):ok("visualElementShown",
b,k)));a.i.delete(c.layer||0);a.j=void 0;e=p(a.u);for(k=e.next();!k.done;k=e.next())b=p(k.value),k=b.next().value,b=b.next().value,b.has(c.layer)&&d&&Vs(a,k,d,c.layer);for(c=0;c<a.l.length;c++){e=a.l[c];try{e()}catch(m){Rp(m)}}for(c=a.l.length=0;c<a.m.length;c++){e=a.m[c];try{e()}catch(m){Rp(m)}}}
;function Ys(){var a,b;return w(function(c){if(1==c.h)return a=ps.h,a?t(c,rs(a),2):(Sp(Error("InnertubeTransportService unavailable in fetchDatasyncIds")),c.return(void 0));if(b=c.i)return b.errorMetadata?(Rp(Error("Datasync IDs fetch responded with "+b.errorMetadata.code+": "+b.error)),c.return(void 0)):c.return(b.Go);Sp(Error("Network request to get Datasync IDs failed."));return c.return(void 0)})}
;var Zs=x.caches,$s;function at(a){var b=a.indexOf(":");return-1===b?{Cb:a}:{Cb:a.substring(0,b),datasyncId:a.substring(b+1)}}
function bt(){return w(function(a){if(void 0!==$s)return a.return($s);$s=new Promise(function(b){var c;return w(function(d){switch(d.h){case 1:return wa(d,2),t(d,Zs.open("test-only"),4);case 4:return t(d,Zs.delete("test-only"),5);case 5:xa(d,3);break;case 2:if(c=ya(d),c instanceof Error&&"SecurityError"===c.name)return b(!1),d.return();case 3:b("caches"in window),d.h=0}})});
return a.return($s)})}
function ct(a){var b,c,d,e,f,g,h;w(function(k){if(1==k.h)return t(k,bt(),2);if(3!=k.h){if(!k.i)return k.return(!1);b=[];return t(k,Zs.keys(),3)}c=k.i;d=p(c);for(e=d.next();!e.done;e=d.next())f=e.value,g=at(f),h=g.datasyncId,!h||a.includes(h)||b.push(Zs.delete(f));return k.return(Promise.all(b).then(function(m){return m.some(function(q){return q})}))})}
function dt(){var a,b,c,d,e,f,g;return w(function(h){if(1==h.h)return t(h,bt(),2);if(3!=h.h){if(!h.i)return h.return(!1);a=sk("cache contains other");return t(h,Zs.keys(),3)}b=h.i;c=p(b);for(d=c.next();!d.done;d=c.next())if(e=d.value,f=at(e),(g=f.datasyncId)&&g!==a)return h.return(!0);return h.return(!1)})}
;function et(){try{return!!self.localStorage}catch(a){return!1}}
;function ft(a){a=a.match(/(.*)::.*::.*/);if(null!==a)return a[1]}
function gt(a){if(et()){var b=Object.keys(window.localStorage);b=p(b);for(var c=b.next();!c.done;c=b.next()){c=c.value;var d=ft(c);void 0===d||a.includes(d)||self.localStorage.removeItem(c)}}}
function ht(){if(!et())return!1;var a=sk(),b=Object.keys(window.localStorage);b=p(b);for(var c=b.next();!c.done;c=b.next())if(c=ft(c.value),void 0!==c&&c!==a)return!0;return!1}
;function jt(){Ys().then(function(a){a&&(Cl(a),ct(a),gt(a))})}
function kt(){var a=new Pm;bf.N(function(){var b,c,d,e;return w(function(f){switch(f.h){case 1:if(M("ytidb_clear_optimizations_killswitch")){f.s(2);break}b=sk("clear");if(b.startsWith("V")){var g=[b];Cl(g);ct(g);gt(g);return f.return()}c=ht();return t(f,dt(),3);case 3:return d=f.i,t(f,Dl(),4);case 4:if(e=f.i,!c&&!d&&!e)return f.return();case 2:a.H()?jt():a.l.add("publicytnetworkstatus-online",jt,!0,void 0,void 0),f.h=0}})})}
;function lt(a){a&&(a.dataset?a.dataset[mt("loaded")]="true":a.setAttribute("data-loaded","true"))}
function nt(a,b){return a?a.dataset?a.dataset[mt(b)]:a.getAttribute("data-"+b):null}
var ot={};function mt(a){return ot[a]||(ot[a]=String(a).replace(/\-([a-z])/g,function(b,c){return c.toUpperCase()}))}
;var pt=/\.vflset|-vfl[a-zA-Z0-9_+=-]+/,qt=/-[a-zA-Z]{2,3}_[a-zA-Z]{2,3}(?=(\/|$))/;function rt(a,b,c){c=void 0===c?null:c;if(window.spf&&spf.script){c="";if(a){var d=a.indexOf("jsbin/"),e=a.lastIndexOf(".js"),f=d+6;-1<d&&-1<e&&e>f&&(c=a.substring(f,e),c=c.replace(pt,""),c=c.replace(qt,""),c=c.replace("debug-",""),c=c.replace("tracing-",""))}spf.script.load(a,c,b)}else st(a,b,c)}
function st(a,b,c){c=void 0===c?null:c;var d=tt(a),e=document.getElementById(d),f=e&&nt(e,"loaded"),g=e&&!f;f?b&&b():(b&&(f=Qn(d,b),b=""+Sa(b),ut[b]=f),g||(e=vt(a,d,function(){nt(e,"loaded")||(lt(e),Tn(d),Ci(Ya(Un,d),0))},c)))}
function vt(a,b,c,d){d=void 0===d?null:d;var e=Md("SCRIPT");e.id=b;e.onload=function(){c&&setTimeout(c,0)};
e.onreadystatechange=function(){switch(e.readyState){case "loaded":case "complete":e.onload()}};
d&&e.setAttribute("nonce",d);Id(e,Pf(a));a=document.getElementsByTagName("head")[0]||document.body;a.insertBefore(e,a.firstChild);return e}
function wt(a){a=tt(a);var b=document.getElementById(a);b&&(Un(a),b.parentNode.removeChild(b))}
function xt(a,b){a&&b&&(a=""+Sa(b),(a=ut[a])&&Sn(a))}
function tt(a){var b=document.createElement("a");Vb(b,a);a=b.href.replace(/^[a-zA-Z]+:\/\//,"//");return"js-"+$b(a)}
var ut={};var zt=[],At=!1;function Bt(){if(!M("disable_biscotti_fetch_for_ad_blocker_detection")&&!M("disable_biscotti_fetch_entirely_for_all_web_clients")&&xq()&&"1"!=rb()){var a=function(){At=!0;"google_ad_status"in window?Zh("DCLKSTAT",1):Zh("DCLKSTAT",2)};
try{rt("//static.doubleclick.net/instream/ad_status.js",a)}catch(b){}zt.push(bf.N(function(){if(!(At||"google_ad_status"in window)){try{xt("//static.doubleclick.net/instream/ad_status.js",a)}catch(b){}At=!0;Zh("DCLKSTAT",3)}},5E3))}}
function Ct(){var a=Number(A("DCLKSTAT",0));return isNaN(a)?0:a}
;function Dt(){this.state=1;this.h=null}
l=Dt.prototype;
l.initialize=function(a,b,c){if(a.program){var d,e=null!=(d=a.interpreterScript)?d:null,f;d=null!=(f=a.interpreterUrl)?f:null;a.interpreterSafeScript&&(e=a.interpreterSafeScript,Db("From proto message. b/166824318"),e=e.privateDoNotAccessOrElseSafeScriptWrappedValue||"",e=(f=Ab())?f.createScript(e):e,e=(new Fb(e)).toString());a.interpreterSafeUrl&&(d=a.interpreterSafeUrl,Db("From proto message. b/166824318"),d=Jb(d.privateDoNotAccessOrElseTrustedResourceUrlWrappedValue||"").toString());Et(this,e,
d,a.program,b,c)}else Sp(Error("Cannot initialize botguard without program"))};
function Et(a,b,c,d,e,f){var g=void 0===g?"trayride":g;c?(a.state=2,rt(c,function(){window[g]?Ht(a,d,g,e):(a.state=3,wt(c),Sp(new Q("Unable to load Botguard","from "+c)))},f)):b?(f=Md("SCRIPT"),f.textContent=b,f.nonce=Yb(),document.head.appendChild(f),document.head.removeChild(f),window[g]?Ht(a,d,g,e):(a.state=4,Sp(new Q("Unable to load Botguard from JS")))):Sp(new Q("Unable to load VM; no url or JS provided"))}
l.isInitialized=function(){return!!this.h};
l.getState=function(){return this.state};
function Ht(a,b,c,d){a.state=5;try{var e=new Bd({program:b,globalName:c});e.wc.then(function(){a.state=6;d&&d(b)});
It(a,e)}catch(f){a.state=7,f instanceof Error&&Sp(f)}}
l.invoke=function(a){a=void 0===a?{}:a;if(this.h){var b=this.h;a={pb:a};if(b.i)throw Error("Already disposed");b=b.l([a.pb,a.yc])}else b=null;return b};
l.dispose=function(){It(this,null);this.state=8};
function It(a,b){pe(a.h);a.h=b}
;var Jt=new Dt;function Kt(){return Jt.isInitialized()}
function Lt(a){a=void 0===a?{}:a;return Jt.invoke(a)}
;function Mt(a){var b=this;var c=void 0===c?0:c;var d=void 0===d?dk():d;this.l=c;this.j=d;this.i=new Ad;this.h=a;a={};c=p(this.h.entries());for(d=c.next();!d.done;a={sa:a.sa,Ba:a.Ba},d=c.next()){var e=p(d.value);d=e.next().value;e=e.next().value;a.Ba=d;a.sa=e;d=function(f){return function(){f.sa.eb();b.h[f.Ba].Ra=!0;b.h.every(function(g){return!0===g.Ra})&&b.i.resolve()}}(a);
e=Zj(d,Nt(this,a.sa));this.h[a.Ba]=Object.assign({},a.sa,{eb:d,Na:e})}}
function Ot(a){var b=Array.from(a.h.keys()).sort(function(d,e){return Nt(a,a.h[e])-Nt(a,a.h[d])});
b=p(b);for(var c=b.next();!c.done;c=b.next())c=a.h[c.value],void 0===c.Na||c.Ra||(a.j.T(c.Na),Zj(c.eb,10))}
Mt.prototype.cancel=function(){for(var a=p(this.h),b=a.next();!b.done;b=a.next())b=b.value,void 0===b.Na||b.Ra||this.j.T(b.Na),b.Ra=!0;this.i.resolve()};
function Nt(a,b){var c;return null!=(c=b.priority)?c:a.l}
;function Pt(a){this.state=a;this.plugins=[];this.m=void 0}
Pt.prototype.install=function(){this.plugins.push.apply(this.plugins,fa(Ja.apply(0,arguments)))};
Pt.prototype.transition=function(a,b){var c=this,d=this.C.find(function(f){return f.from===c.state&&f.B===a});
if(d){this.j&&(Ot(this.j),this.j=void 0);this.state=a;d=d.action.bind(this);var e=this.plugins.filter(function(f){return f[a]}).map(function(f){return f[a]});
d(Qt(this,e,this.m),b)}else throw Error("no transition specified from "+this.state+" to "+a);};
function Qt(a,b,c){return function(){var d=Ja.apply(0,arguments),e=b.filter(function(k){var m;return 10===(null!=(m=null!=c?c:k.priority)?m:0)}),f=b.filter(function(k){var m;
return 10!==(null!=(m=null!=c?c:k.priority)?m:0)});
dk();var g={};e=p(e);for(var h=e.next();!h.done;g={Ca:g.Ca},h=e.next())g.Ca=h.value,bk(function(k){return function(){k.Ca.ka.apply(k.Ca,fa(d))}}(g));
f=f.map(function(k){var m;return{eb:function(){k.ka.apply(k,fa(d))},
priority:null!=(m=null!=c?c:k.priority)?m:0}});
f.length&&(a.j=new Mt(f))}}
da.Object.defineProperties(Pt.prototype,{currentState:{configurable:!0,enumerable:!0,get:function(){return this.state}}});function Rt(a){Pt.call(this,void 0===a?"document_active":a);var b=this;this.m=10;this.h=new Map;this.C=[{from:"document_active",B:"document_disposed_preventable",action:this.u},{from:"document_active",B:"document_disposed",action:this.l},{from:"document_disposed_preventable",B:"document_disposed",action:this.l},{from:"document_disposed_preventable",B:"flush_logs",action:this.o},{from:"document_disposed_preventable",B:"document_active",action:this.i},{from:"document_disposed",B:"flush_logs",action:this.o},
{from:"document_disposed",B:"document_active",action:this.i},{from:"document_disposed",B:"document_disposed",action:function(){}},
{from:"flush_logs",B:"document_active",action:this.i}];window.addEventListener("pagehide",function(c){b.transition("document_disposed",{event:c})});
window.addEventListener("beforeunload",function(c){b.transition("document_disposed_preventable",{event:c})})}
r(Rt,Pt);Rt.prototype.u=function(a,b){if(!this.h.get("document_disposed_preventable")){a(null==b?void 0:b.event);var c,d;if((null==b?0:null==(c=b.event)?0:c.defaultPrevented)||(null==b?0:null==(d=b.event)?0:d.returnValue)){b.event.returnValue||(b.event.returnValue=!0);b.event.defaultPrevented||b.event.preventDefault();this.h=new Map;this.transition("document_active");return}}this.h.set("document_disposed_preventable",!0);this.h.get("document_disposed")?this.transition("flush_logs"):this.transition("document_disposed")};
Rt.prototype.l=function(a,b){this.h.get("document_disposed")?this.transition("document_active"):(a(null==b?void 0:b.event),this.h.set("document_disposed",!0),this.transition("flush_logs"))};
Rt.prototype.o=function(a,b){a(null==b?void 0:b.event);this.transition("document_active")};
Rt.prototype.i=function(){this.h=new Map};function St(a){Pt.call(this,void 0===a?"document_visibility_unknown":a);var b=this;this.C=[{from:"document_visibility_unknown",B:"document_visible",action:this.i},{from:"document_visibility_unknown",B:"document_hidden",action:this.h},{from:"document_visibility_unknown",B:"document_foregrounded",action:this.o},{from:"document_visibility_unknown",B:"document_backgrounded",action:this.l},{from:"document_visible",B:"document_hidden",action:this.h},{from:"document_visible",B:"document_foregrounded",action:this.o},
{from:"document_visible",B:"document_visible",action:this.i},{from:"document_foregrounded",B:"document_visible",action:this.i},{from:"document_foregrounded",B:"document_hidden",action:this.h},{from:"document_foregrounded",B:"document_foregrounded",action:this.o},{from:"document_hidden",B:"document_visible",action:this.i},{from:"document_hidden",B:"document_backgrounded",action:this.l},{from:"document_hidden",B:"document_hidden",action:this.h},{from:"document_backgrounded",B:"document_hidden",action:this.h},
{from:"document_backgrounded",B:"document_backgrounded",action:this.l},{from:"document_backgrounded",B:"document_visible",action:this.i}];document.addEventListener("visibilitychange",function(c){"visible"===document.visibilityState?b.transition("document_visible",{event:c}):b.transition("document_hidden",{event:c})});
M("visibility_lifecycles_dynamic_backgrounding")&&(window.addEventListener("blur",function(c){b.transition("document_backgrounded",{event:c})}),window.addEventListener("focus",function(c){b.transition("document_foregrounded",{event:c})}))}
r(St,Pt);St.prototype.i=function(a,b){a(null==b?void 0:b.event);M("visibility_lifecycles_dynamic_backgrounding")&&this.transition("document_foregrounded")};
St.prototype.h=function(a,b){a(null==b?void 0:b.event);M("visibility_lifecycles_dynamic_backgrounding")&&this.transition("document_backgrounded")};
St.prototype.l=function(a,b){a(null==b?void 0:b.event)};
St.prototype.o=function(a,b){a(null==b?void 0:b.event)};function Tt(){this.h=new Rt;this.i=new St}
Tt.prototype.install=function(){var a=Ja.apply(0,arguments);this.h.install.apply(this.h,fa(a));this.i.install.apply(this.i,fa(a))};function Ut(){Tt.call(this);var a={};this.install((a.document_disposed={ka:this.j},a));a={};this.install((a.flush_logs={ka:this.l},a))}
var Vt;r(Ut,Tt);Ut.prototype.l=function(){ok("finalPayload",{csn:jq()})};
Ut.prototype.j=function(){Wp(Xp)};function Wt(){}
Wt.getInstance=function(){var a=z("ytglobal.storage_");a||(a=new Wt,y("ytglobal.storage_",a,void 0));return a};
Wt.prototype.estimate=function(){var a,b,c;return w(function(d){a=navigator;return(null==(b=a.storage)?0:b.estimate)?d.return(a.storage.estimate()):(null==(c=a.webkitTemporaryStorage)?0:c.queryUsageAndQuota)?d.return(Xt()):d.return()})};
function Xt(){var a=navigator;return new Promise(function(b,c){var d;null!=(d=a.webkitTemporaryStorage)&&d.queryUsageAndQuota?a.webkitTemporaryStorage.queryUsageAndQuota(function(e,f){b({usage:e,quota:f})},function(e){c(e)}):c(Error("webkitTemporaryStorage is not supported."))})}
y("ytglobal.storageClass_",Wt,void 0);function mk(a,b){var c=this;this.handleError=a;this.h=b;this.i=!1;void 0===self.document||self.addEventListener("beforeunload",function(){c.i=!0});
this.j=Math.random()<=ci("ytidb_transaction_ended_event_rate_limit",.02)}
mk.prototype.logEvent=function(a,b){switch(a){case "IDB_DATA_CORRUPTED":M("idb_data_corrupted_killswitch")||this.h("idbDataCorrupted",b);break;case "IDB_UNEXPECTEDLY_CLOSED":this.h("idbUnexpectedlyClosed",b);break;case "IS_SUPPORTED_COMPLETED":M("idb_is_supported_completed_killswitch")||this.h("idbIsSupportedCompleted",b);break;case "QUOTA_EXCEEDED":Yt(this,b);break;case "TRANSACTION_ENDED":this.j&&this.h("idbTransactionEnded",b);break;case "TRANSACTION_UNEXPECTEDLY_ABORTED":a=Object.assign({},b,
{hasWindowUnloaded:this.i}),this.h("idbTransactionAborted",a)}};
function Yt(a,b){Wt.getInstance().estimate().then(function(c){c=Object.assign({},b,{isSw:void 0===self.document,isIframe:self!==self.top,deviceStorageUsageMbytes:Zt(null==c?void 0:c.usage),deviceStorageQuotaMbytes:Zt(null==c?void 0:c.quota)});a.h("idbQuotaExceeded",c)})}
function Zt(a){return"undefined"===typeof a?"-1":String(Math.ceil(a/1048576))}
;var $t=window;
function au(){var a=$t.uaChPolyfill.state;if(0===a.type)ok("clientHintsPolyfillEvent",{clientHintsSupported:!1});else{var b=navigator.userAgent,c=void 0!==a.syntheticUa&&a.syntheticUa===b,d={clientHintsSupported:!0,uaAccessedBeforePolyfill:a.didAccessUaBeforePolyfillAvailable,syntheticUaMatches:c};a.didAccessUaBeforePolyfillAvailable&&(d.uaAccessBeforePolyfillCount=a.uaAccessBeforePolyfillCount,a.firstAccessUaError&&(d.firstUaAccessStack=String(a.firstAccessUaError.stack).replace(/\n/g,""),Rp(a.firstAccessUaError)),
d.polyfillAvailabilityDelayMs=a.polyfillAvailabilityDelay);ok("clientHintsPolyfillEvent",d);c||(b={syntheticUa:a.syntheticUa,ua:b},b.brand=a.data.brands.map(function(e){return'"'+e.brand+'"; v="'+e.version+'"'}),b.mobileness=a.data.mobile,a=a.data.values,a.architecture&&(b.platformArchitecture=a.architecture),a.model&&(b.model=a.model),a.platform&&(b.platformBrand=a.platform),a.platformVersion&&(b.platformVersion=a.platformVersion),a.uaFullVersion&&(b.fullVersion=a.uaFullVersion),ok("clientHintsPolyfillDiagnostics",
b))}}
var bu=!1;function cu(){var a;1===(null==(a=$t.uaChPolyfill)?void 0:a.state.type)?bu||($t.uaChPolyfill.onReady=cu,bu=!0):$t.uaChPolyfill&&au()}
;function du(a,b,c){K.call(this);var d=this;c=c||A("POST_MESSAGE_ORIGIN",void 0)||window.document.location.protocol+"//"+window.document.location.hostname;this.j=b||null;this.L="*";this.l=c;this.sessionId=null;this.channel="widget";this.M=!!a;this.A=function(e){a:if(!("*"!=d.l&&e.origin!=d.l||d.j&&e.source!=d.j||"string"!==typeof e.data)){try{var f=JSON.parse(e.data)}catch(g){break a}if(!(null==f||d.M&&(d.sessionId&&d.sessionId!=f.id||d.channel&&d.channel!=f.channel))&&f)switch(f.event){case "listening":"null"!=
e.origin&&(d.l=d.L=e.origin);d.j=e.source;d.sessionId=f.id;d.i&&(d.i(),d.i=null);break;case "command":d.m&&(!d.u||0<=eb(d.u,f.func))&&d.m(f.func,f.args,e.origin)}}};
this.u=this.i=this.m=null;window.addEventListener("message",this.A)}
r(du,K);du.prototype.sendMessage=function(a,b){if(b=b||this.j){this.sessionId&&(a.id=this.sessionId);this.channel&&(a.channel=this.channel);try{var c=JSON.stringify(a);b.postMessage(c,this.L)}catch(d){ji(d)}}};
du.prototype.I=function(){window.removeEventListener("message",this.A);K.prototype.I.call(this)};function eu(){this.i=[];this.isReady=!1;this.j={};var a=this.h=new du(!!A("WIDGET_ID_ENFORCE")),b=this.ic.bind(this);a.m=b;a.u=null;this.h.channel="widget";if(a=A("WIDGET_ID"))this.h.sessionId=a}
l=eu.prototype;l.ic=function(a,b,c){"addEventListener"===a&&b?(a=b[0],this.j[a]||"onReady"===a||(this.addEventListener(a,fu(this,a)),this.j[a]=!0)):this.kb(a,b,c)};
l.kb=function(){};
function fu(a,b){return function(c){return a.sendMessage(b,c)}}
l.addEventListener=function(){};
l.Vb=function(){this.isReady=!0;this.sendMessage("initialDelivery",this.Xa());this.sendMessage("onReady");fb(this.i,this.Ib,this);this.i=[]};
l.Xa=function(){return null};
function gu(a,b){a.sendMessage("infoDelivery",b)}
l.Ib=function(a){this.isReady?this.h.sendMessage(a):this.i.push(a)};
l.sendMessage=function(a,b){this.Ib({event:a,info:void 0===b?null:b})};
l.dispose=function(){this.h=null};function hu(a){return(0===a.search("cue")||0===a.search("load"))&&"loadModule"!==a}
function iu(a,b,c){if("string"===typeof a)return{videoId:a,startSeconds:b,suggestedQuality:c};b=["endSeconds","startSeconds","mediaContentUrl","suggestedQuality","videoId"];c={};for(var d=0;d<b.length;d++){var e=b[d];a[e]&&(c[e]=a[e])}return c}
function ju(a,b,c,d){if(Ra(a)&&!Array.isArray(a)){b="playlist list listType index startSeconds suggestedQuality".split(" ");c={};for(d=0;d<b.length;d++){var e=b[d];a[e]&&(c[e]=a[e])}return c}b={index:b,startSeconds:c,suggestedQuality:d};"string"===typeof a&&16===a.length?b.list="PL"+a:b.playlist=a;return b}
;function ku(a){eu.call(this);this.listeners=[];this.api=a;this.addEventListener("onReady",this.onReady.bind(this));this.addEventListener("onVideoProgress",this.tc.bind(this));this.addEventListener("onVolumeChange",this.uc.bind(this));this.addEventListener("onApiChange",this.mc.bind(this));this.addEventListener("onPlaybackQualityChange",this.pc.bind(this));this.addEventListener("onPlaybackRateChange",this.qc.bind(this));this.addEventListener("onStateChange",this.sc.bind(this));this.addEventListener("onWebglSettingsChanged",
this.vc.bind(this))}
r(ku,eu);l=ku.prototype;
l.kb=function(a,b,c){if(this.api.isExternalMethodAvailable(a,c)){b=b||[];if(0<b.length&&hu(a)){var d=b;if(Ra(d[0])&&!Array.isArray(d[0]))var e=d[0];else switch(e={},a){case "loadVideoById":case "cueVideoById":e=iu(d[0],void 0!==d[1]?Number(d[1]):void 0,d[2]);break;case "loadVideoByUrl":case "cueVideoByUrl":e=d[0];"string"===typeof e&&(e={mediaContentUrl:e,startSeconds:void 0!==d[1]?Number(d[1]):void 0,suggestedQuality:d[2]});b:{if((d=e.mediaContentUrl)&&(d=/\/([ve]|embed)\/([^#?]+)/.exec(d))&&d[2]){d=
d[2];break b}d=null}e.videoId=d;e=iu(e);break;case "loadPlaylist":case "cuePlaylist":e=ju(d[0],d[1],d[2],d[3])}b.length=1;b[0]=e}this.api.handleExternalCall(a,b,c);hu(a)&&gu(this,this.Xa())}};
l.onReady=function(){var a=this.Vb.bind(this);this.h.i=a};
l.addEventListener=function(a,b){this.listeners.push({eventType:a,listener:b});this.api.addEventListener(a,b)};
l.Xa=function(){if(!this.api)return null;var a=this.api.getApiInterface();kb(a,"getVideoData");for(var b={apiInterface:a},c=0,d=a.length;c<d;c++){var e=a[c];if(0===e.search("get")||0===e.search("is")){var f=0;0===e.search("get")?f=3:0===e.search("is")&&(f=2);f=e.charAt(f).toLowerCase()+e.substr(f+1);try{var g=this.api[e]();b[f]=g}catch(h){}}}b.videoData=this.api.getVideoData();b.currentTimeLastUpdated_=Date.now()/1E3;return b};
l.sc=function(a){a={playerState:a,currentTime:this.api.getCurrentTime(),duration:this.api.getDuration(),videoData:this.api.getVideoData(),videoStartBytes:0,videoBytesTotal:this.api.getVideoBytesTotal(),videoLoadedFraction:this.api.getVideoLoadedFraction(),playbackQuality:this.api.getPlaybackQuality(),availableQualityLevels:this.api.getAvailableQualityLevels(),currentTimeLastUpdated_:Date.now()/1E3,playbackRate:this.api.getPlaybackRate(),mediaReferenceTime:this.api.getMediaReferenceTime()};this.api.getVideoUrl&&
(a.videoUrl=this.api.getVideoUrl());this.api.getVideoContentRect&&(a.videoContentRect=this.api.getVideoContentRect());this.api.getProgressState&&(a.progressState=this.api.getProgressState());this.api.getPlaylist&&(a.playlist=this.api.getPlaylist());this.api.getPlaylistIndex&&(a.playlistIndex=this.api.getPlaylistIndex());this.api.getStoryboardFormat&&(a.storyboardFormat=this.api.getStoryboardFormat());gu(this,a)};
l.pc=function(a){gu(this,{playbackQuality:a})};
l.qc=function(a){gu(this,{playbackRate:a})};
l.mc=function(){for(var a=this.api.getOptions(),b={namespaces:a},c=0,d=a.length;c<d;c++){var e=a[c],f=this.api.getOptions(e);b[e]={options:f};for(var g=0,h=f.length;g<h;g++){var k=f[g],m=this.api.getOption(e,k);b[e][k]=m}}this.sendMessage("apiInfoDelivery",b)};
l.uc=function(){gu(this,{muted:this.api.isMuted(),volume:this.api.getVolume()})};
l.tc=function(a){a={currentTime:a,videoBytesLoaded:this.api.getVideoBytesLoaded(),videoLoadedFraction:this.api.getVideoLoadedFraction(),currentTimeLastUpdated_:Date.now()/1E3,playbackRate:this.api.getPlaybackRate(),mediaReferenceTime:this.api.getMediaReferenceTime()};this.api.getProgressState&&(a.progressState=this.api.getProgressState());gu(this,a)};
l.vc=function(){var a={sphericalProperties:this.api.getSphericalProperties()};gu(this,a)};
l.dispose=function(){eu.prototype.dispose.call(this);for(var a=0;a<this.listeners.length;a++){var b=this.listeners[a];this.api.removeEventListener(b.eventType,b.listener)}this.listeners=[]};function lu(a){K.call(this);this.i={};this.started=!1;this.connection=a;this.connection.subscribe("command",this.Eb,this)}
r(lu,K);l=lu.prototype;l.start=function(){this.started||this.h()||(this.started=!0,this.connection.la("RECEIVING"))};
l.la=function(a,b){this.started&&!this.h()&&this.connection.la(a,b)};
l.Eb=function(a,b,c){if(this.started&&!this.h()){var d=b||{};switch(a){case "addEventListener":"string"===typeof d.event&&this.addListener(d.event);break;case "removeEventListener":"string"===typeof d.event&&this.removeListener(d.event);break;default:this.api.isReady()&&this.api.isExternalMethodAvailable(a,c||null)&&(b=mu(a,b||{}),c=this.api.handleExternalCall(a,b,c||null),(c=nu(a,c))&&this.la(a,c))}}};
l.addListener=function(a){if(!(a in this.i)){var b=this.nc.bind(this,a);this.i[a]=b;this.addEventListener(a,b)}};
l.nc=function(a,b){this.started&&!this.h()&&this.connection.la(a,this.Wa(a,b))};
l.Wa=function(a,b){if(null!=b)return{value:b}};
l.removeListener=function(a){a in this.i&&(this.removeEventListener(a,this.i[a]),delete this.i[a])};
l.I=function(){var a=this.connection;a.h()||rg(a.i,"command",this.Eb,this);this.connection=null;for(var b in this.i)this.i.hasOwnProperty(b)&&this.removeListener(b);K.prototype.I.call(this)};function ou(a,b){lu.call(this,b);this.api=a;this.start()}
r(ou,lu);ou.prototype.addEventListener=function(a,b){this.api.addEventListener(a,b)};
ou.prototype.removeEventListener=function(a,b){this.api.removeEventListener(a,b)};
function mu(a,b){switch(a){case "loadVideoById":return a=iu(b),[a];case "cueVideoById":return a=iu(b),[a];case "loadVideoByPlayerVars":return[b];case "cueVideoByPlayerVars":return[b];case "loadPlaylist":return a=ju(b),[a];case "cuePlaylist":return a=ju(b),[a];case "seekTo":return[b.seconds,b.allowSeekAhead];case "playVideoAt":return[b.index];case "setVolume":return[b.volume];case "setPlaybackQuality":return[b.suggestedQuality];case "setPlaybackRate":return[b.suggestedRate];case "setLoop":return[b.loopPlaylists];
case "setShuffle":return[b.shufflePlaylist];case "getOptions":return[b.module];case "getOption":return[b.module,b.option];case "setOption":return[b.module,b.option,b.value];case "handleGlobalKeyDown":return[b.keyCode,b.shiftKey,b.ctrlKey,b.altKey,b.metaKey,b.key,b.code]}return[]}
function nu(a,b){switch(a){case "isMuted":return{muted:b};case "getVolume":return{volume:b};case "getPlaybackRate":return{playbackRate:b};case "getAvailablePlaybackRates":return{availablePlaybackRates:b};case "getVideoLoadedFraction":return{videoLoadedFraction:b};case "getPlayerState":return{playerState:b};case "getCurrentTime":return{currentTime:b};case "getPlaybackQuality":return{playbackQuality:b};case "getAvailableQualityLevels":return{availableQualityLevels:b};case "getDuration":return{duration:b};
case "getVideoUrl":return{videoUrl:b};case "getVideoEmbedCode":return{videoEmbedCode:b};case "getPlaylist":return{playlist:b};case "getPlaylistIndex":return{playlistIndex:b};case "getOptions":return{options:b};case "getOption":return{option:b}}}
ou.prototype.Wa=function(a,b){switch(a){case "onReady":return;case "onStateChange":return{playerState:b};case "onPlaybackQualityChange":return{playbackQuality:b};case "onPlaybackRateChange":return{playbackRate:b};case "onError":return{errorCode:b}}return lu.prototype.Wa.call(this,a,b)};
ou.prototype.I=function(){lu.prototype.I.call(this);delete this.api};function pu(a){a=void 0===a?!1:a;K.call(this);this.i=new L(a);re(this,Ya(pe,this.i))}
$a(pu,K);pu.prototype.subscribe=function(a,b,c){return this.h()?0:this.i.subscribe(a,b,c)};
pu.prototype.l=function(a,b){this.h()||this.i.ja.apply(this.i,arguments)};function qu(a,b,c){pu.call(this);this.j=a;this.destination=b;this.id=c}
r(qu,pu);qu.prototype.la=function(a,b){this.h()||this.j.la(this.destination,this.id,a,b)};
qu.prototype.I=function(){this.destination=this.j=null;pu.prototype.I.call(this)};function ru(a,b,c){K.call(this);this.destination=a;this.origin=c;this.i=Dn(window,"message",this.j.bind(this));this.connection=new qu(this,a,b);re(this,Ya(pe,this.connection))}
r(ru,K);ru.prototype.la=function(a,b,c,d){this.h()||a!==this.destination||(a={id:b,command:c},d&&(a.data=d),this.destination.postMessage(Rf(a),this.origin))};
ru.prototype.j=function(a){var b;if(b=!this.h())if(b=a.origin===this.origin)a:{b=this.destination;do{b:{var c=a.source;do{if(c===b){c=!0;break b}if(c===c.parent)break;c=c.parent}while(null!=c);c=!1}if(c){b=!0;break a}b=b.opener}while(null!=b);b=!1}if(b&&(b=a.data,"string"===typeof b)){try{b=JSON.parse(b)}catch(d){return}b.command&&(c=this.connection,c.h()||c.l("command",b.command,b.data,a.origin))}};
ru.prototype.I=function(){En(this.i);this.destination=null;K.prototype.I.call(this)};function su(a){a=a||{};var b={},c={};this.url=a.url||"";this.args=a.args||vb(b);this.assets=a.assets||{};this.attrs=a.attrs||vb(c);this.fallback=a.fallback||null;this.fallbackMessage=a.fallbackMessage||null;this.html5=!!a.html5;this.disable=a.disable||{};this.loaded=!!a.loaded;this.messages=a.messages||{}}
su.prototype.clone=function(){var a=new su,b;for(b in this)if(this.hasOwnProperty(b)){var c=this[b];"object"==Pa(c)?a[b]=vb(c):a[b]=c}return a};var tu=/cssbin\/(?:debug-)?([a-zA-Z0-9_-]+?)(?:-2x|-web|-rtl|-vfl|.css)/;function uu(a){a=a||"";if(window.spf){var b=a.match(tu);spf.style.load(a,b?b[1]:"",void 0)}else vu(a)}
function vu(a){var b=wu(a),c=document.getElementById(b),d=c&&nt(c,"loaded");d||c&&!d||(c=xu(a,b,function(){nt(c,"loaded")||(lt(c),Tn(b),Ci(Ya(Un,b),0))}))}
function xu(a,b,c){var d=document.createElement("link");d.id=b;d.onload=function(){c&&setTimeout(c,0)};
a=Pf(a);Wb(d,a);(document.getElementsByTagName("head")[0]||document.body).appendChild(d);return d}
function wu(a){var b=Md("A");Db("This URL is never added to the DOM");Vb(b,new Lb(a,Mb));a=b.href.replace(/^[a-zA-Z]+:\/\//,"//");return"css-"+$b(a)}
;function yu(){K.call(this);this.i=[]}
r(yu,K);yu.prototype.I=function(){for(;this.i.length;){var a=this.i.pop();a.target.removeEventListener(a.name,a.ka,void 0)}K.prototype.I.call(this)};function zu(){yu.apply(this,arguments)}
r(zu,yu);function Au(a,b,c,d){K.call(this);var e=this;this.M=b;this.webPlayerContextConfig=d;this.Ta=!1;this.api={};this.Da=this.u=null;this.S=new L;this.i={};this.fa=this.Ea=this.elementId=this.Ua=this.config=null;this.Y=!1;this.l=this.A=null;this.Fa={};this.Mb=["onReady"];this.lastError=null;this.lb=NaN;this.L={};this.Nb=new zu(this);this.na=0;this.j=this.m=a;re(this,Ya(pe,this.S));Bu(this);Cu(this);re(this,Ya(pe,this.Nb));c?this.na=Ci(function(){e.loadNewVideoConfig(c)},0):d&&(Du(this),Eu(this))}
r(Au,K);l=Au.prototype;l.getId=function(){return this.M};
l.loadNewVideoConfig=function(a){if(!this.h()){this.na&&(Di(this.na),this.na=0);var b=a||{};b instanceof su||(b=new su(b));this.config=b;this.setConfig(a);Eu(this);this.isReady()&&Fu(this)}};
function Du(a){var b;a.webPlayerContextConfig?b=a.webPlayerContextConfig.rootElementId:b=a.config.attrs.id;a.elementId=b||a.elementId;"video-player"===a.elementId&&(a.elementId=a.M,a.webPlayerContextConfig?a.webPlayerContextConfig.rootElementId=a.M:a.config.attrs.id=a.M);var c;(null==(c=a.j)?void 0:c.id)===a.elementId&&(a.elementId+="-player",a.webPlayerContextConfig?a.webPlayerContextConfig.rootElementId=a.elementId:a.config.attrs.id=a.elementId)}
l.setConfig=function(a){this.Ua=a;this.config=Gu(a);Du(this);if(!this.Ea){var b;this.Ea=Hu(this,(null==(b=this.config.args)?void 0:b.jsapicallback)||"onYouTubePlayerReady")}this.config.args?this.config.args.jsapicallback=null:this.config.args={jsapicallback:null};var c;if(null==(c=this.config)?0:c.attrs)a=this.config.attrs,(b=a.width)&&this.j&&(this.j.style.width=Xd(Number(b)||b)),(a=a.height)&&this.j&&(this.j.style.height=Xd(Number(a)||a))};
function Fu(a){if(a.config&&!0!==a.config.loaded)if(a.config.loaded=!0,!a.config.args||"0"!==a.config.args.autoplay&&0!==a.config.args.autoplay&&!1!==a.config.args.autoplay){var b;a.api.loadVideoByPlayerVars(null!=(b=a.config.args)?b:null)}else a.api.cueVideoByPlayerVars(a.config.args)}
function Iu(a){var b=!0,c=Ju(a);c&&a.config&&(a=Ku(a),b=nt(c,"version")===a);return b&&!!z("yt.player.Application.create")}
function Eu(a){if(!a.h()&&!a.Y){var b=Iu(a);if(b&&"html5"===(Ju(a)?"html5":null))a.fa="html5",a.isReady()||Lu(a);else if(Mu(a),a.fa="html5",b&&a.l&&a.m)a.m.appendChild(a.l),Lu(a);else{a.config&&(a.config.loaded=!0);var c=!1;a.A=function(){c=!0;var d=Nu(a,"player_bootstrap_method")?z("yt.player.Application.createAlternate")||z("yt.player.Application.create"):z("yt.player.Application.create");var e=a.config?Gu(a.config):void 0;d&&d(a.m,e,a.webPlayerContextConfig);Lu(a)};
a.Y=!0;b?a.A():(rt(Ku(a),a.A),(b=Ou(a))&&uu(b),Pu(a)&&!c&&y("yt.player.Application.create",null,void 0))}}}
function Ju(a){var b=Ld(a.elementId);!b&&a.j&&a.j.querySelector&&(b=a.j.querySelector("#"+a.elementId));return b}
function Lu(a){if(!a.h()){var b=Ju(a),c=!1;b&&b.getApiInterface&&b.getApiInterface()&&(c=!0);if(c){a.Y=!1;if(!Nu(a,"html5_remove_not_servable_check_killswitch")){var d;if((null==b?0:b.isNotServable)&&a.config&&(null==b?0:b.isNotServable(null==(d=a.config.args)?void 0:d.video_id)))return}Qu(a)}else a.lb=Ci(function(){Lu(a)},50)}}
function Qu(a){Bu(a);a.Ta=!0;var b=Ju(a);if(b){a.u=Ru(a,b,"addEventListener");a.Da=Ru(a,b,"removeEventListener");var c=b.getApiInterface();c=c.concat(b.getInternalApiInterface());for(var d=a.api,e=0;e<c.length;e++){var f=c[e];d[f]||(d[f]=Ru(a,b,f))}}for(var g in a.i)a.i.hasOwnProperty(g)&&a.u&&a.u(g,a.i[g]);Fu(a);a.Ea&&a.Ea(a.api);a.S.ja("onReady",a.api)}
function Ru(a,b,c){var d=b[c];return function(){var e=Ja.apply(0,arguments);try{return a.lastError=null,d.apply(b,e)}catch(f){"sendAbandonmentPing"!==c&&(f.params=c,a.lastError=f,Sp(f))}}}
function Bu(a){a.Ta=!1;if(a.Da)for(var b in a.i)a.i.hasOwnProperty(b)&&a.Da(b,a.i[b]);for(var c in a.L)a.L.hasOwnProperty(c)&&Di(Number(c));a.L={};a.u=null;a.Da=null;b=a.api;for(var d in b)b.hasOwnProperty(d)&&(b[d]=null);b.addEventListener=function(e,f){a.addEventListener(e,f)};
b.removeEventListener=function(e,f){a.removeEventListener(e,f)};
b.destroy=function(){a.dispose()};
b.getLastError=function(){return a.getLastError()};
b.getPlayerType=function(){return a.getPlayerType()};
b.getCurrentVideoConfig=function(){return a.Ua};
b.loadNewVideoConfig=function(e){a.loadNewVideoConfig(e)};
b.isReady=function(){return a.isReady()}}
l.isReady=function(){return this.Ta};
function Cu(a){a.addEventListener("WATCH_LATER_VIDEO_ADDED",function(b){Tn("WATCH_LATER_VIDEO_ADDED",b)});
a.addEventListener("WATCH_LATER_VIDEO_REMOVED",function(b){Tn("WATCH_LATER_VIDEO_REMOVED",b)});
a.addEventListener("onAdAnnounce",function(b){Tn("a11y-announce",b)})}
l.addEventListener=function(a,b){var c=this,d=Hu(this,b);d&&(0<=eb(this.Mb,a)||this.i[a]||(b=Su(this,a),this.u&&this.u(a,b)),this.S.subscribe(a,d),"onReady"===a&&this.isReady()&&Ci(function(){d(c.api)},0))};
l.removeEventListener=function(a,b){this.h()||(b=Hu(this,b))&&rg(this.S,a,b)};
function Hu(a,b){var c=b;if("string"===typeof b){if(a.Fa[b])return a.Fa[b];c=function(){var d=Ja.apply(0,arguments),e=z(b);if(e)try{e.apply(x,d)}catch(f){Rp(f)}};
a.Fa[b]=c}return c?c:null}
function Su(a,b){var c="ytPlayer"+b+a.M;a.i[b]=c;x[c]=function(d){var e=Ci(function(){if(!a.h()){a.S.ja(b,null!=d?d:void 0);var f=a.L,g=String(e);g in f&&delete f[g]}},0);
qb(a.L,String(e))};
return c}
l.getPlayerType=function(){return this.fa||(Ju(this)?"html5":null)};
l.getLastError=function(){return this.lastError};
function Mu(a){a.cancel();Bu(a);a.fa=null;a.config&&(a.config.loaded=!1);var b=Ju(a);b&&(Iu(a)||!Pu(a)?a.l=b:(b&&b.destroy&&b.destroy(),a.l=null));if(a.m)for(a=a.m;b=a.firstChild;)a.removeChild(b)}
l.cancel=function(){this.A&&xt(Ku(this),this.A);Di(this.lb);this.Y=!1};
l.I=function(){Mu(this);if(this.l&&this.config&&this.l.destroy)try{this.l.destroy()}catch(b){Rp(b)}this.Fa=null;for(var a in this.i)this.i.hasOwnProperty(a)&&(x[this.i[a]]=null);this.Ua=this.config=this.api=null;delete this.m;delete this.j;K.prototype.I.call(this)};
function Pu(a){var b,c;a=null==(b=a.config)?void 0:null==(c=b.args)?void 0:c.fflags;return!!a&&-1!==a.indexOf("player_destroy_old_version=true")}
function Ku(a){return a.webPlayerContextConfig?a.webPlayerContextConfig.jsUrl:(a=a.config.assets)?a.js:""}
function Ou(a){return a.webPlayerContextConfig?a.webPlayerContextConfig.cssUrl:(a=a.config.assets)?a.css:""}
function Nu(a,b){if(a.webPlayerContextConfig)var c=a.webPlayerContextConfig.serializedExperimentFlags;else{var d;if(null==(d=a.config)?0:d.args)c=a.config.args.fflags}return"true"===pi(c||"","&")[b]}
function Gu(a){for(var b={},c=p(Object.keys(a)),d=c.next();!d.done;d=c.next()){d=d.value;var e=a[d];b[d]="object"===typeof e?vb(e):e}return b}
;var Tu={},Uu="player_uid_"+(1E9*Math.random()>>>0);function Vu(a,b,c){var d="player";c=void 0===c?!0:c;d="string"===typeof d?Ld(d):d;var e=Uu+"_"+Sa(d),f=Tu[e];if(f&&c)return Wu(a,b)?f.api.loadVideoByPlayerVars(a.args||null):f.loadNewVideoConfig(a),f.api;f=new Au(d,e,a,b);Tu[e]=f;Tn("player-added",f.api);re(f,function(){delete Tu[f.getId()]});
return f.api}
function Wu(a,b){return b&&b.serializedExperimentFlags?b.serializedExperimentFlags.includes("web_player_remove_playerproxy=true"):a&&a.args&&a.args.fflags?a.args.fflags.includes("web_player_remove_playerproxy=true"):!1}
;var Xu=null,Yu=null,Zu=null;function $u(){var a=Xu.getVideoData(1);a=a.title?a.title+" - YouTube":"YouTube";document.title!==a&&(document.title=a)}
;function av(a,b,c){a="ST-"+$b(a).toString(36);b=b?gc(b):"";c=c||5;xq()&&oj(a,b,c)}
;function bv(a,b,c){b=void 0===b?{}:b;c=void 0===c?!1:c;var d=A("EVENT_ID");d&&(b.ei||(b.ei=d));if(b){d=a;var e=void 0===e?!0:e;var f=A("VALID_SESSION_TEMPDATA_DOMAINS",[]),g=dc(window.location.href);g&&f.push(g);g=dc(d);if(0<=eb(f,g)||!g&&0==d.lastIndexOf("/",0))if(M("autoescape_tempdata_url")&&(f=document.createElement("a"),Vb(f,d),d=f.href),d&&(d=ec(d),f=d.indexOf("#"),d=0>f?d:d.substr(0,f)))if(e&&!b.csn&&(b.itct||b.ved)&&(b=Object.assign({csn:jq()},b)),h){var h=parseInt(h,10);isFinite(h)&&0<h&&
av(d,b,h)}else av(d,b)}if(c)return!1;if((window.ytspf||{}).enabled)spf.navigate(a);else{var k=void 0===k?{}:k;var m=void 0===m?"":m;var q=void 0===q?window:q;c=q.location;a=hc(a,k)+m;var u=void 0===u?Wd:u;a:{u=void 0===u?Wd:u;for(k=0;k<u.length;++k)if(m=u[k],m instanceof Ud&&m.isValid(a)){u=new Fd(a,Dd);break a}u=void 0}c.href=Hd(u||Gd)}return!0}
;y("yt.setConfig",Zh,void 0);y("yt.config.set",Zh,void 0);y("yt.setMsg",oq,void 0);y("yt.msgs.set",oq,void 0);y("yt.logging.errors.log",Rp,void 0);
y("writeEmbed",function(){var a=A("PLAYER_CONFIG",void 0);if(!a){var b=A("PLAYER_VARS",void 0);b&&(a={args:b})}Fq(!0);"gvn"===a.args.ps&&(document.body.style.backgroundColor="transparent");a.attrs||(a.attrs={width:"100%",height:"100%",id:"video-player"});var c=document.referrer;b=A("POST_MESSAGE_ORIGIN");window!==window.top&&c&&c!==document.URL&&(a.args.loaderUrl=c);M("embeds_js_api_set_1p_cookie")&&(c=ui(window.location.href),c.embedsTokenValue&&(a.args.embedsTokenValue=c.embedsTokenValue));cs();
if((c=A("WEB_PLAYER_CONTEXT_CONFIGS",void 0))&&"WEB_PLAYER_CONTEXT_CONFIG_ID_EMBEDDED_PLAYER"in c){c=c.WEB_PLAYER_CONTEXT_CONFIG_ID_EMBEDDED_PLAYER;if(!c.serializedForcedExperimentIds){var d=ui(window.location.href);d.forced_experiments&&(c.serializedForcedExperimentIds=d.forced_experiments)}Xu=Vu(a,c,!1)}else Xu=Vu(a);Xu.addEventListener("onVideoDataChange",$u);a=A("POST_MESSAGE_ID","player");A("ENABLE_JS_API")?Zu=new ku(Xu):A("ENABLE_POST_API")&&"string"===typeof a&&"string"===typeof b&&(Yu=new ru(window.parent,
a,b),Zu=new ou(Xu,Yu.connection));Bt();M("ytidb_create_logger_embed_killswitch")||lk();M("flush_gel_on_teardown")&&(a={},Vt||(Vt=new Ut),Vt.install((a.flush_logs={ka:function(){Zo()}},a)));
M("networkless_logging_web_embedded")&&(M("embeds_web_enable_new_nwl")?Wm():dn());M("embeds_enable_ua_ch_polyfill")&&cu();M("ytidb_clear_embedded_player")&&bf.N(function(){if(!Js){Pq||(Pq=new Mq);var e=Pq;if(M("web_inject_fetch_manager")){var f={hc:Is,Kb:Hs};e.providers.set(f.hc,f)}f={ob:{feedbackEndpoint:$q(Ds),modifyChannelNotificationPreferenceEndpoint:$q(Es),playlistEditEndpoint:$q(Fs),subscribeEndpoint:$q(Bs),unsubscribeEndpoint:$q(Cs),webPlayerShareEntityServiceEndpoint:$q(Gs)}};var g=M("web_enable_client_location_service")?
Wq.getInstance():void 0,h={};g&&(h.client_location=g);if(void 0===k){Gq.h||(Gq.h=new Gq);var k=Gq.h}if(void 0===m)if(M("web_inject_fetch_manager"))var m=e.resolve(Is);else Hs.h||(Hs.h=new Hs),m=Hs.h;qs(f,m,k,h);Js=ps.h}kt()})},void 0);
var cv=hi(function(){hs();var a=sj.getInstance(),b=vj(119),c=1<window.devicePixelRatio;if(document.body&&Af(document.body,"exp-invert-logo"))if(c&&!Af(document.body,"inverted-hdpi")){var d=document.body;if(d.classList)d.classList.add("inverted-hdpi");else if(!Af(d,"inverted-hdpi")){var e=yf(d);zf(d,e+(0<e.length?" inverted-hdpi":"inverted-hdpi"))}}else!c&&Af(document.body,"inverted-hdpi")&&Bf();if(b!=c){b="f"+(Math.floor(119/31)+1);d=wj(b)||0;d=c?d|67108864:d&-67108865;0==d?delete rj[b]:(c=d.toString(16),
rj[b]=c.toString());c=!0;M("web_secure_pref_cookie_killswitch")&&(c=!1);b=a.h;d=[];for(var f in rj)d.push(f+"="+encodeURIComponent(String(rj[f])));oj(b,d.join("&"),63072E3,a.i,c)}Ts.h||(Ts.h=new Ts);a=Ts.h;f=16623;var g=void 0===g?{}:g;Object.values(pq).includes(f)||(Sp(new Q("createClientScreen() called with a non-page VE",f)),f=83769);g.isHistoryNavigation||a.h.push({rootVe:f,key:g.key||""});a.o=[];a.C=[];g.rb?Ws(a,f,g):Xs(a,f,g)}),dv=hi(function(){Xu&&Xu.sendAbandonmentPing&&Xu.sendAbandonmentPing();
A("PL_ATT")&&Jt.dispose();for(var a=bf,b=0,c=zt.length;b<c;b++)a.T(zt[b]);zt.length=0;wt("//static.doubleclick.net/instream/ad_status.js");At=!1;Zh("DCLKSTAT",0);qe(Zu,Yu);Xu&&(Xu.removeEventListener("onVideoDataChange",$u),Xu.destroy())});
window.addEventListener?(window.addEventListener("load",cv),M("embeds_web_enable_replace_unload_w_pagehide")?window.addEventListener("pagehide",dv):window.addEventListener("unload",dv)):window.attachEvent&&(window.attachEvent("onload",cv),window.attachEvent("onunload",dv));Za("yt.abuse.player.botguardInitialized",z("yt.abuse.player.botguardInitialized")||Kt);Za("yt.abuse.player.invokeBotguard",z("yt.abuse.player.invokeBotguard")||Lt);
Za("yt.abuse.dclkstatus.checkDclkStatus",z("yt.abuse.dclkstatus.checkDclkStatus")||Ct);Za("yt.player.exports.navigate",z("yt.player.exports.navigate")||bv);Za("yt.util.activity.init",z("yt.util.activity.init")||In);Za("yt.util.activity.getTimeSinceActive",z("yt.util.activity.getTimeSinceActive")||Ln);Za("yt.util.activity.setTimestamp",z("yt.util.activity.setTimestamp")||Jn);}).call(this);
