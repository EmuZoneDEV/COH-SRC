/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#if ENABLE(3D_CANVAS)

#include "JSWebGLBuffer.h"

#include "WebGLBuffer.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSWebGLBuffer);

/* Hash table for prototype */

static const HashTableValue JSWebGLBufferPrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSWebGLBufferPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSWebGLBufferPrototypeTableValues, 0 };
#else
    { 1, 0, JSWebGLBufferPrototypeTableValues, 0 };
#endif

const ClassInfo JSWebGLBufferPrototype::s_info = { "WebGLBufferPrototype", 0, &JSWebGLBufferPrototypeTable, 0 };

JSObject* JSWebGLBufferPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSWebGLBuffer>(exec, globalObject);
}

const ClassInfo JSWebGLBuffer::s_info = { "WebGLBuffer", 0, 0, 0 };

JSWebGLBuffer::JSWebGLBuffer(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<WebGLBuffer> impl)
    : DOMObjectWithGlobalPointer(structure, globalObject)
    , m_impl(impl)
{
}

JSWebGLBuffer::~JSWebGLBuffer()
{
    forgetDOMObject(this, impl());
}

JSObject* JSWebGLBuffer::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSWebGLBufferPrototype(JSWebGLBufferPrototype::createStructure(globalObject->objectPrototype()));
}

JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, WebGLBuffer* object)
{
    return getDOMObjectWrapper<JSWebGLBuffer>(exec, globalObject, object);
}
WebGLBuffer* toWebGLBuffer(JSC::JSValue value)
{
    return value.inherits(&JSWebGLBuffer::s_info) ? static_cast<JSWebGLBuffer*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(3D_CANVAS)
