#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <stdio.h>
#include <nan.h>

#define GF2_DIM 32

unsigned long gf2_matrix_times(unsigned long *mat, unsigned long vec) {
    unsigned long sum = 0;
    while (vec) {
        if (vec & 1) {
            sum ^= *mat;
        }
        vec >>= 1;
        mat++;
    }
    return sum;
}

void gf2_matrix_square(unsigned long *square, unsigned long *mat) {
    int n;
    for (n = 0; n < GF2_DIM; n++) {
        square[n] = gf2_matrix_times(mat, mat[n]);
    }
}

unsigned long crc32_combine(unsigned long crc1, unsigned long crc2, long len2) {
    int n;
    unsigned long row;
    unsigned long even[GF2_DIM];    /* even-power-of-two zeros operator */
    unsigned long odd[GF2_DIM];     /* odd-power-of-two zeros operator */

    /* degenerate case (also disallow negative lengths) */
    if (len2 <= 0) {
        return crc1;
    }

    /* put operator for one zero bit in odd */
    odd[0] = 0xedb88320UL;   /* CRC-32 polynomial */
    row = 1;
    for (n = 1; n < GF2_DIM; n++) {
        odd[n] = row;
        row <<= 1;
    }

    /* put operator for two zero bits in even */
    gf2_matrix_square(even, odd);

    /* put operator for four zero bits in odd */
    gf2_matrix_square(odd, even);

    /* apply len2 zeros to crc1 (first square will put the operator for one
       zero byte, eight zero bits, in even) */
    do {
        /* apply zeros operator for this bit of len2 */
        gf2_matrix_square(even, odd);
        if (len2 & 1) {
            crc1 = gf2_matrix_times(even, crc1);
        }
        len2 >>= 1;

        /* if no more bits set, then done */
        if (len2 == 0) {
            break;
        }
        
        /* another iteration of the loop with odd and even swapped */
        gf2_matrix_square(odd, even);
        if (len2 & 1) {
            crc1 = gf2_matrix_times(odd, crc1);
        }
        len2 >>= 1;

        /* if no more bits set, then done */
    } while (len2 != 0);

    /* return combined crc */
    crc1 ^= crc2;
    return crc1;
}



using namespace v8;
using namespace node;

NAN_METHOD(crc32_combine) {
	NanScope();

	if (args.Length() < 3) {
		NanThrowTypeError("Wrong number of arguments");
		NanReturnUndefined();
	}

	if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber()) {
		NanThrowTypeError("Wrong arguments");
		NanReturnUndefined();
	}

	unsigned long combine = crc32_combine(
		args[0]->NumberValue(), // crc32 #1
		args[1]->NumberValue(), // crc32 #2
		args[2]->NumberValue()  // len2
		);

	NanReturnValue(NanNewBufferHandle((char *)&combine, sizeof(unsigned long)));
}

NAN_METHOD(crc32_combine_multi) {
	NanScope();

	if (args.Length() < 1) {
		NanThrowTypeError("Wrong number of arguments");
		NanReturnUndefined();
	}

	if (!args[0]->IsArray()) {
		NanThrowTypeError("Wrong arguments");
		NanReturnUndefined();
	}

	Local<Array> arr = Local<Array>::Cast(args[0]);
	uint32_t arLength = arr->Length();

	if (arLength < 2) {
		NanThrowTypeError("Array too small. I need min 2 elements");
		NanReturnUndefined();
	}

	Local<Object> firstElementCrc = Local<Object>::Cast(arr->Get(0));
	unsigned long retCrc = firstElementCrc->Get(NanNew("crc"))->Uint32Value();
	unsigned long retLen = firstElementCrc->Get(NanNew("len"))->Uint32Value();

	uint32_t n;
	for (n = 1; n < arLength; n++){
		Local<Object> obj = Local<Object>::Cast(arr->Get(n));
		unsigned long crc1 = obj->Get(NanNew("crc"))->Uint32Value();
		unsigned long len2 = obj->Get(NanNew("len"))->Uint32Value();
		retCrc = crc32_combine(retCrc, crc1, len2);
		retLen += len2;
	}

	int length = sizeof(unsigned long);
	Local<Object> crcBuffer = NanNewBufferHandle((char *)&retCrc, length);

	Local<Object> lengthBuffer = NanNewBufferHandle((char *)&retLen, length);

	Local<Number> numRetLen = NanNew<Number>(retLen);

	Local<Object> retValObj = NanNew<Object>();
	retValObj->Set(NanNew("combinedCrc32"), crcBuffer);
	retValObj->Set(NanNew("intLength"), numRetLen);
	retValObj->Set(NanNew("bufferLength"), lengthBuffer);

	NanReturnValue(retValObj);
}

void init(Handle<Object> exports) {
	exports->Set(NanNew("crc32_combine"), NanNew<FunctionTemplate>(crc32_combine)->GetFunction());
	exports->Set(NanNew("crc32_combine_multi"), NanNew<FunctionTemplate>(crc32_combine_multi)->GetFunction());
}

NODE_MODULE(crc32, init)

